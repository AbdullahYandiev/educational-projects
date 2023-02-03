import numpy as np

class MinMaxScaler:
    def fit(self, data):
        """Store calculated statistics

        Parameters:
        data (np.array): train set, size (num_obj, num_features)
        """
        self.data = np.row_stack([data.min(axis=0), data.max(axis=0)])
        
    def transform(self, data):
        """
        Parameters:
        data (np.array): train set, size (num_obj, num_features)

        Return:
        np.array: scaled data, size (num_obj, num_features)
        """
        return (data - self.data[0])/(self.data[1] - self.data[0])


class StandardScaler:
    def fit(self, data):
        """Store calculated statistics
        
        Parameters:
        data (np.array): train set, size (num_obj, num_features)
        """
        self.data = np.row_stack([data.mean(axis=0), data.var(axis=0) ** 0.5])
        
    def transform(self, data):
        """
        Parameters:
        data (np.array): train set, size (num_obj, num_features)

        Return:
        np.array: scaled data, size (num_obj, num_features)
        """
        return (data - self.data[0]) / self.data[1]