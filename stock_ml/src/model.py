import torch
from torch import nn
from torch._prims_common import DeviceLikeType
import pandas as pd

class MultiLinearRegression(nn.Module):
    def __init__(self,
                 epochs:int = 100,
                 device:DeviceLikeType = 'cpu',
                 input_size:int = 8,
                 output_size:int = 1,
                 layers:int = 2):
        super(MultiLinearRegression,self).__init()
        self.device =device
        self.epochs = epochs
        layers = []
        #first layer
        layers += [
            nn.Linear(input_size,layers*input_size),
            nn.LayerNorm(layers*input_size),
        ]
        #hidden layers
        for _ in range(layers):
            layers += [
                nn.Linear(layers*input_size),
                nn.BatchNorm1d(layers*input_size),
            ]
        #last layer
        layers += [
            nn.Linear(layers*input_size,output_size)
        ]
        self.model = nn.Sequential(*layers).to(self.device)
        self.loss_fn = nn.MSELoss()
        self.optimizer = torch.optim.SGD(self.model.parameters(),lr=0.01,momentum=0.9)

    def foward(self,x):
        return self.model(x)

    def fit(self, x, y):
        x_dev = torch.tensor(x).to(self._device)
        y_dev = torch.tensor(y).to(self._device)

        self._model.train()
        for _ in range(self._epochs):
            self.optimizer.zero_grad()

            y_pred = self.model(x_dev)
            loss = self.loss_fn(y_pred, y_dev)
            loss.backward()
            self.optimizer.step()

    def predict(self, x, cols=None):
        if cols is not None:
            x = x[cols]

        x_dev = torch.tensor(x).float().to(self._device)

        self.model.eval()
        with torch.no_grad():
            prediction = self.model(x_dev)
            return prediction.cpu().numpy()
