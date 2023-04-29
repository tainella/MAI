import numpy as np

class Point_Generator():
    
    def __init__(self, num_points: int, percent_outliers: float) -> None:
        self.num_points = num_points
        self.percent_outliers = percent_outliers
        self.inliers_num = int(np.floor(self.num_points * (1 - self.percent_outliers)))
        self.outliers_num = int(np.floor(self.num_points * self.percent_outliers))
    
    def generate_case(self, k: float = 1., b: float = 0., eps:float = 0.1) -> np.ndarray:
        """Генерация точек выборки вместе с шумовыми для прямой у = kx + b + eps,
        где eps - случайный шум.
        Args:
        k (float, optional): Параметр наклона прямой. Defaults to 1…
        b (float, optional): Параметр сдвига прямой. Defaults to 0…
        eps (float, optional): Параметр разброса точек вдоль прямой. Defaults to 0.1.
        Returns:
        пр.ndarray: Обучающая выборка.
        """

        if k is None:
            k = p.random.uniform(-1, 1)
        
        if b is None:
            b = np.random.uniform(0, 5)
        x = np.linspace(0, 10, self.inliers_num)
        y = k + x + b + np.random.normal(scale=eps)
        inliers = np.vstack((x, y)).T
        
        x = np.random.uniform(0, 10, self.outliers_num)
        y = np.random.uniform(y.min(), y.max(), self.outliers_num)
        outliers = np.vstack((x, y)).T

        data = np.concatenate((inliers, outliers))
        np.random.shuffle(data)
        return data