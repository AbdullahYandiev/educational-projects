import numpy as np


def prod_non_zero_diag(X: np.ndarray) -> int:
    """
    Compute product of nonzero elements from matrix diagonal, 
    return -1 if there is no such elements.
    
    Return type: int / np.integer / np.int32 / np.int64
    """
    if np.all(np.diag(X) == 0):
        return -1
    lst = np.diag(X).copy()
    lst[lst == 0] = 1
    return np.prod(lst)


def are_multisets_equal(x: np.ndarray, y: np.ndarray) -> bool:
    """
    Return True if both 1-d arrays create equal multisets, False if not.
    
    Return type: bool / np.bool_
    """
    return (np.sort(x) == np.sort(y)).all()
    

def max_after_zero(x: np.ndarray) -> int:
    """
    Find max element after zero in 1-d array, 
    return -1 if there is no such elements.

    Return type: int / np.integer / np.int32 / np.int64
    """
    if np.all(x[:-1] != 0):
        return -1
    return x[1:][(x == 0)[:-1]].max()


def convert_image(image: np.ndarray, weights: np.ndarray) -> np.ndarray:
    """
    Sum up image channels with weights.

    Return type: np.ndarray
    """
    return np.sum(image * weights, axis=2)


def run_length_encoding(x: np.ndarray) -> (np.ndarray, np.ndarray):
    """
    Make run-length encoding.

    Return type: (np.ndarray, np.ndarray)
    """
    mask = np.append(True, x[1:] != x[:-1]) 
    return x[mask], np.diff(np.append(np.where(mask), len(x)))


def pairwise_distance(X: np.ndarray, Y: np.ndarray) -> np.ndarray:
    """
    Return pairwise object distance.

    Return type: np.ndarray
    """
    return np.sqrt(np.add.outer(np.sum(X ** 2, axis=1), np.sum(Y ** 2, axis=1)) - 2 * np.dot(X, Y.T))
