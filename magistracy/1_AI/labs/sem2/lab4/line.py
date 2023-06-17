'''
Модуль для работы с прямыми
'''

import numpy as np

class Line():
    def __init__(self, points: np.ndarray) -> None:
        self.k = None
        self.b = None
        self.points = points

    def estimate_params(self) -> None:
        points_num = len(self.points)
        if points_num < 2:
            raise ValueError(f"Not enough points. Must be at least 2, but got {points_num}.")
        elif points_num == 2 or points_num == 5:
            x_points = [p[0] for p in self.points]
            y_points = [p[1] for p in self.points]
            A = np.vstack([x_points, np.ones(len(x_points))]).T
            self.k, self.b = np.linalg.lstsq(A, y_points, rcond=None)[0]
        else:
            raise NotImplementedError

    def distance(self, x, y, mode = 'mse'):
        if mode == 'mse':
            return (y - x * self.k - self.b) ** 2
        else:
            return()

    def divide_points(self, points: np.ndarray, eps: float, mode = 'mse') -> tuple:
        '''
        Разделение массива points на inliers и outliers
        '''
        inliers = []
        outliers = []
        for p in points:
            if self.distance(p[0], p[1], mode = mode) < eps:
                inliers.append(p)
            else:
                outliers.append(p)
        return (inliers, outliers)