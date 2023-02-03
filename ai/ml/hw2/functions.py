from typing import List


def prod_non_zero_diag(X: List[List[int]]) -> int:
    """
    Compute product of nonzero elements from matrix diagonal, 
    return -1 if there is no such elements.

    Return type: int
    """    
    lst = [X[i][i] for i in range(min(len(X), len(X[0]))) if X[i][i]]
    if len(lst) == 0:
        return -1
    res = 1
    for i in lst:
        res *= i
    return res


def are_multisets_equal(x: List[int], y: List[int]) -> bool:
    """
    Return True if both 1-d arrays create equal multisets, False if not.

    Return type: bool
    """
    d = {}
    for i in x:
        d[i] = 1 if i not in d else d[i] + 1
    for i in y:
        if i not in d or d[i] == 0:
            return False
        else:
            d[i] -= 1
    return True


def max_after_zero(x: List[int]) -> int:
    """
    Find max element after zero in 1-d array, 
    return -1 if there is no such elements.

    Return type: int
    """
    max = pred = -1
    for i in x:
        if pred == 0 and max < i:
            max = i
        pred = i
    return max 


def convert_image(image: List[List[List[float]]], weights: List[float]) -> List[List[float]]:
    """
    Sum up image channels with weights.

    Return type: List[List[float]]
    """
    res = []
    for i in image:
        lst = []
        for j in i:
            sum = 0
            for n, k in enumerate(j):
                sum += k * weights[n]
            lst.append(sum)
        res.append(lst)
    return res


def run_length_encoding(x: List[int]) -> (List[int], List[int]):
    """
    Make run-length encoding.

    Return type: (List[int], List[int])
    """
    res_e, res_n, pred, cur_n = [], [], x[0], 1
    res_e.append(x[0])
    for i in x[1:]:
        if i != pred:
            res_e.append(i)
            res_n.append(cur_n)
            cur_n = 1
            pred = i
        else:
            cur_n += 1
    res_n.append(cur_n)
    return res_e, res_n


def pairwise_distance(X: List[List[float]], Y: List[List[float]]) -> List[List[float]]:
    """
    Return pairwise object distance.

    Return type: List[List[float]]
    """
    res = []
    for i in X:
        lst = []
        for j in Y:
            s = 0
            for k in range(len(j)):
                s += (i[k] - j[k]) ** 2
            lst.append(s ** (0.5))
        res.append(lst)
    return res
