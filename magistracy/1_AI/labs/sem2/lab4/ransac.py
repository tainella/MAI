'''
RANSAC for 2d line modules

Algo:
I Hypothesis generation stage
1. Sample 2d points (1. 2 points; 2. 5 points)
2. Model estimation (1. analytics; 2. MSE estimation)

II Hypothesis evaluation stage
3. Inlier counting (%inlinear > thresh)
    if True -> best params
    if False -> 1.
4. # iter > num_iter?
'''

import numpy as np
from matplotlib import pyplot as plt

from line import Line

class RANSAC:
    """
    RANSAC algo
    1. Сэмплировать 2д точки (2 точки; 5 точек)
    2. Определение модели (аналитически; mse)
    3. Подсчет inliers (% от общего числа > thres). Если проходит порог, best_params, если нет - шаг 1
    4. проверка числа итераций

    """
    def __init__(self) -> None:
        self.iter_num: int = 1000
        self.online_thresh: float = 0
        self.mode = 'mse'
        self.epsilon: float = 0.01
        self.linier_points = 2
        self.best_params: dict = {}
        self.inliers: list = []
        self.outliers: list = []
        self.points: np.ndarray = None
    
    def set_case(self, case_params) -> None:
        try:
            self.points = case_params['points']
        except:
            NotImplementedError
        
        if 'iter_num' in case_params.keys():
            self.iter_num = case_params['iter_num']
        if 'online_thresh' in case_params.keys():
            self.online_thresh = case_params['online_thresh']
        if 'mode' in case_params.keys():
            self.mode = case_params['mode']
        if 'linier_points' in case_params.keys():
            self.linier_points = case_params['linier_points']
        if 'epsilon' in case_params.keys():
            self.epsilon = case_params['epsilon']
    
    def clear_case(self) -> None:
        self.__init__()
    
    def fit(self):
        for i in range(self.iter_num):
            ind_samples = np.random.choice(range(len(self.points)), self.linier_points)
            selected_points = self.points[ind_samples]
            line = Line(selected_points)
            line.estimate_params()

            inliers, outliers = line.divide_points(self.points, self.epsilon, self.mode)
            score = len(inliers) / len(self.points)
            if score >= self.online_thresh:
                k, b = line.k, line.b
                self.inliers = inliers
                self.outliers = outliers
                self.best_params = {'k' : k, 'b' : b}
                self.online_thresh = score
    
    def draw(self):
        if len(self.inliers) > 0:
            print('Текущий процент:', self.online_thresh)
            plt.scatter([p[0] for p in self.inliers], [p[1] for p in self.inliers], c = 'green')
            plt.scatter([p[0] for p in self.outliers], [p[1] for p in self.outliers], c = 'red')
            x_points = np.array([p[0] for p in self.points])
            plt.plot(x_points, self.best_params['k']*x_points + self.best_params['b'], 'b')
        else:
            print('Алгоритм не нашел решение с заданным порогом')
            print('Текущий процент:', self.online_thresh)
