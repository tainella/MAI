from mpi4py import MPI

comm = MPI.COMM_WORLD
my_rank = comm.Get_rank()
p = comm.Get_size()

if my_rank != 0:
    message = f"Hello from {my_rank}"
    comm.send(message, dest = 0)
else:
    for procid in range(1, p):
        message = comm.recv(source = procid)
        print(f"Processor {my_rank} recieved message from {procid}")
        print(f"Message: {message}")

MPI.Finalize