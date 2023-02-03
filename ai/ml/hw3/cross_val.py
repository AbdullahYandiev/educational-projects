import numpy as np
from collections import defaultdict

def kfold_split(num_objects, num_folds):
    """Split [0, 1, ..., num_objects - 1] into equal num_folds folds (last fold can be longer) and returns num_folds train-val 
       pairs of indexes.

    Parameters:
    num_objects (int): number of objects in train set
    num_folds (int): number of folds for cross-validation split

    Returns:
    list((tuple(np.array, np.array))): list of length num_folds, where i-th element of list contains tuple of 2 numpy arrays,
                                       the 1st numpy array contains all indexes without i-th fold while the 2nd one contains
                                       i-th fold
    """
    indexes = np.array(range(0, num_objects))
    res, k1 = [], 0
    for num_fold in range(num_folds):
        k2 = k1 + num_objects // num_folds if num_fold != num_folds - 1 else num_objects
        res.append((np.append(indexes[:k1], indexes[k2:]), indexes[k1:k2]))
        k1 += num_objects // num_folds
    return res


def knn_cv_score(X, y, parameters, score_function, folds, knn_class):
    """Takes train data, counts cross-validation score over grid of parameters (all possible parameters combinations) 

    Parameters:
    X (2d np.array): train set
    y (1d np.array): train labels
    parameters (dict): dict with keys from {n_neighbors, metrics, weights, normalizers}, values of type list,
                       parameters['normalizers'] contains tuples (normalizer, normalizer_name), see parameters
                       example in your jupyter notebook
    score_function (callable): function with input (y_true, y_predict) which outputs score metric
    folds (list): output of kfold_split
    knn_class (obj): class of knn model to fit

    Returns:
    dict: key - tuple of (normalizer_name, n_neighbors, metric, weight), value - mean score over all folds
    """
    res = {}

    for normalizer in parameters['normalizers']:
        XX = []
        for fold in folds:
            X_train, X_test = X[fold[0]], X[fold[1]]
            if normalizer[0] is not None:
                normalizer[0].fit(X_train)
                X_train = normalizer[0].transform(X_train)
                X_test = normalizer[0].transform(X_test)  
            XX.append((X_train, X_test))

        for n_neighbor in parameters['n_neighbors']:
            for metric in parameters['metrics']:
                for weigth in parameters['weights']:
                    KNN = knn_class(n_neighbors=n_neighbor, weights=weigth, metric=metric)
                    scores = []
                    for i, fold in enumerate(folds):
                        KNN.fit(XX[i][0], y[fold[0]])
                        scores.append(score_function(y[fold[1]], KNN.predict(XX[i][1])))
                    res[(normalizer[1], n_neighbor, metric, weigth)] = np.array(scores).mean()
    return res




# from scalers import StandardScaler, MinMaxScaler
# from sklearn import neighbors
# from sklearn.metrics import r2_score

# X_train = np.array([[2, 1, -1], [1, 1, 1], [0.9, -0.25, 7], [1, 2, -3], [0, 0, 0], [2, -1, 0.5]])
# y_train = np.sum(X_train, axis=1)
# parameters = {
#     'n_neighbors': [1, 2, 4],
#     'metrics': ['euclidean', 'cosine'],
#     'weights': ['uniform', 'distance'],
#     'normalizers': [(None, 'None'), (MinMaxScaler(), 'MinMax'), (StandardScaler(), 'Standard')]
# }
# score_function = r2_score
# folds = kfold_split(6, 3)
# res = knn_cv_score(X_train, y_train, parameters, score_function, folds, neighbors.KNeighborsRegressor)
# for i in res:
#     print(i, ':', res[i])


 