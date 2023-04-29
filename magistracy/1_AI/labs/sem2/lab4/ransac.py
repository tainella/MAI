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
from line import Line

class RANSAC:
    "RANSAC algo"
    def __init__(self) -> None:
        self.iter_num: int = 100
        self.online_thresh: float = 0.8
        self.epsilon: float = 0.1
        self.best_params: dict = {}
        self.inliers: list = []
        self.outliers: list = []
        self.score: int = 0
        self. points: np.ndarray = None
    
    def set_case(self, case_params) ->:
        pass
    
    def clear_case(self) -> None:
        pass
    
    def fit(self):
        for i in range(self.iter_num):
            pass
    
    def draw(self):
        pass