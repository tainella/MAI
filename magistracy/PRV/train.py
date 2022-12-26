import torch
import torchvision
import torchvision.transforms as transforms
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
from torchvision.models import vgg16

from mpi4py import MPI

from tqdm import tqdm

def train(my_rank, net, criterion, optimizer, trainloader):
    running_loss = 0.0
    it = 0
    for i, data in tqdm(enumerate(trainloader, 0)):
        if it == 5:
            break
        inputs, labels = data
        optimizer.zero_grad()

        outputs = net(inputs)
        loss = criterion(outputs, labels)
        loss.backward()
        optimizer.step()

        running_loss += loss.item()
        it += 1
    print(f'model: {my_rank} \n loss: {running_loss / it}\n')
    running_loss = 0.0
    return net

if __name__ == "__main__":
    comm = MPI.COMM_WORLD
    my_rank = comm.Get_rank()
    p = comm.Get_size()

    img_size = (300, 450)

    transform = transforms.Compose(
      [transforms.Resize(img_size),
      transforms.ToTensor(),
      transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5))])

    if my_rank == 0:
        trainset = torchvision.datasets.CIFAR10(root='./data', train = True, download=False, transform=transform)
        #разделяем датасет на p-1 частей
        length1 = len(trainset) // (p - 1)
        l = [length1 for i in range(p - 2)]
        l.append(len(trainset) - (p - 2) * length1)
        parts = torch.utils.data.random_split(trainset, l)
        for i in range(len(parts)):
            comm.send(parts[i], dest = i + 1)

    else:
        #создаем свой экземпляр нейросети
        net = vgg16(pretrained=False)
        num_ftrs = net.classifier[6].in_features
        net.classifier[6] = nn.Linear(num_ftrs, 10)
        
        criterion = nn.CrossEntropyLoss()
        optimizer = optim.SGD(net.parameters(), lr=0.001, momentum=0.9)

        #получаем свой кусок датасета
        dataset_part = comm.recv(source = 0)

        #создаем загрузчик датасета
        batch_size = 4
        trainloader = torch.utils.data.DataLoader(dataset_part, batch_size=batch_size, shuffle=True, num_workers=2)

        #обучаем свой экземпляр нейросети
        net = train(my_rank, net, criterion, optimizer, trainloader)

        #сохраняем обученную модель
        PATH = f'./net_{my_rank}.pth'
        torch.save(net.state_dict(), PATH)

    MPI.Finalize