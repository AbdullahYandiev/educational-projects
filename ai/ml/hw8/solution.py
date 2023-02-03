import numpy as np

import sklearn
import sklearn.metrics
from sklearn.cluster import KMeans


def silhouette_score(x, labels):
    '''
    :param np.ndarray x: Непустой двумерный массив векторов-признаков
    :param np.ndarray labels: Непустой одномерный массив меток объектов
    :return float: Коэффициент силуэта для выборки x с метками labels
    '''

    _, labels = np.unique(labels, return_inverse=True)
    n_samples = len(labels)
    label_bin = np.bincount(labels)

    def reducer(D_chunk, start):
        m, n = len(D_chunk), len(label_bin)
        a = np.arange(m)
        dist = np.apply_along_axis(lambda i: np.bincount(labels, weights=D_chunk[i[0]]),
                                   1, a.reshape(-1, 1))
        idx = (a, labels[start:start + m])
        A = dist[idx]
        dist[idx] = np.inf
        dist /= label_bin
        B = dist.min(axis=1)
        return A, B

    A, B = zip(*sklearn.metrics.pairwise_distances_chunked(x, reduce_func=reducer))
    A, B = np.concatenate(A), np.concatenate(B)

    denom = (label_bin - 1).take(labels, mode='clip')
    with np.errstate(divide="ignore", invalid="ignore"):
        A /= denom

    S = B - A
    with np.errstate(divide="ignore", invalid="ignore"):
        S /= np.maximum(A, B)

    sil_score = np.nan_to_num(S).mean()
    return np.nan_to_num(S).mean()


def bcubed_score(true_labels, predicted_labels):
    '''
    :param np.ndarray true_labels: Непустой одномерный массив меток объектов
    :param np.ndarray predicted_labels: Непустой одномерный массив меток объектов
    :return float: B-Cubed для объектов с истинными метками true_labels и предсказанными метками predicted_labels
    '''

    Fvec = np.vectorize(lambda x: ((true_labels == true_labels[x]) *
                                   (predicted_labels == predicted_labels[x])).sum())
    A = Fvec(np.arange(len(true_labels)))

    _, B, C = np.unique(true_labels, return_inverse=True, return_counts=True)
    precision = np.mean(A / C[B])

    _, B, C = np.unique(predicted_labels, return_inverse=True, return_counts=True)
    recall = np.mean(A / C[B])

    score = 2 * (precision * recall) / (precision + recall)
    return score


class KMeansClassifier(sklearn.base.BaseEstimator):
    def __init__(self, n_clusters):
        '''
        :param int n_clusters: Число кластеров которых нужно выделить в обучающей выборке с помощью алгоритма кластеризации
        '''
        super().__init__()
        self.n_clusters = n_clusters

        # Ваш код здесь:＼(º □ º l|l)/

    def fit(self, data, labels):
        '''
            Функция обучает кластеризатор KMeans с заданым числом кластеров, а затем с помощью
        self._best_fit_classification восстанавливает разметку объектов

        :param np.ndarray data: Непустой двумерный массив векторов-признаков объектов обучающей выборки
        :param np.ndarray labels: Непустой одномерный массив. Разметка обучающей выборки. Неразмеченные объекты имеют метку -1.
            Размеченные объекты могут иметь произвольную неотрицательную метку. Существует хотя бы один размеченный объект
        :return KMeansClassifier
        '''
        # Ваш код здесь:＼(º □ º l|l)/

        return self

    def predict(self, data):
        '''
        Функция выполняет предсказание меток класса для объектов, поданных на вход. Предсказание происходит в два этапа
            1. Определение меток кластеров для новых объектов
            2. Преобразование меток кластеров в метки классов с помощью выученного преобразования

        :param np.ndarray data: Непустой двумерный массив векторов-признаков объектов
        :return np.ndarray: Предсказанные метки класса
        '''
        # Ваш код здесь:＼(º □ º l|l)/

        return predictions

    def _best_fit_classification(self, cluster_labels, true_labels):
        '''
        :param np.ndarray cluster_labels: Непустой одномерный массив. Предсказанные метки кластеров.
            Содержит элементы в диапазоне [0, ..., n_clusters - 1]
        :param np.ndarray true_labels: Непустой одномерный массив. Частичная разметка выборки.
            Неразмеченные объекты имеют метку -1. Размеченные объекты могут иметь произвольную неотрицательную метку.
            Существует хотя бы один размеченный объект
        :return
            np.ndarray mapping: Соответствие между номерами кластеров и номерами классов в выборке,
                то есть mapping[idx] -- номер класса для кластера idx
            np.ndarray predicted_labels: Предсказанные в соответствии с mapping метки объектов

            Соответствие между номером кластера и меткой класса определяется как номер класса с максимальным числом объектов
        внутри этого кластера.
            * Если есть несколько классов с числом объектов, равным максимальному, то выбирается метка с наименьшим номером.
            * Если кластер не содержит размеченных объектов, то выбирается номер класса с максимальным числом элементов в выборке.
            * Если же и таких классов несколько, то также выбирается класс с наименьшим номером
        '''

        mask = true_labels != -1
        cl_labels, true_labels = cluster_labels[mask], true_labels[mask]
        labels, counts = np.unique(true_labels, return_counts=True)
        most_frequent = labels[counts == counts.max()].min()
        clusters = np.arange(self.n_clusters)

        def f(cluster):
            idx = np.where(cl_labels == cluster)[0]
            if not len(idx):
                return most_frequent
            labels, counts = np.unique(true_labels[idx], return_counts=True)
            local_frequent = labels[counts == counts.max()].min()
            return local_frequent
        Fvec = np.vectorize(f)

        mapping = Fvec(clusters)
        predicted_labels = mapping[cluster_labels]
        return mapping, predicted_labels
