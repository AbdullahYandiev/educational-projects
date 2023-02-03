import warnings
warnings.filterwarnings('ignore')

from catboost import CatBoostRegressor
import pandas as pd
import numpy as np


def train_model_and_predict(train_file: str, test_file: str) -> np.ndarray:
    """
    This function reads dataset stored in the folder, trains predictor and returns predictions.
    :param train_file: the path to the training dataset
    :param test_file: the path to the testing dataset
    :return: predictions for the test file in the order of the file lines (ndarray of shape (n_samples,))
    """

    df_train = pd.read_json(train_file, lines=True)
    df_test = pd.read_json(test_file, lines=True)
   
    categ_features = []
    for i in range(3):
        categ_features.append(f"actor_{i}_gender")

    y_train = df_train["awards"]
    del df_train["awards"]

    train_size = len(df_train)
    df = pd.concat([df_train, df_test], ignore_index=True)
    df.directors[df.directors == "unknown"] = df.directors[df.directors == "unknown"].apply(lambda x: [])
    df.filming_locations[df.filming_locations == "unknown"] = df.filming_locations[df.filming_locations == "unknown"].apply(lambda x: [])
    df.keywords[df.keywords == "unknown"] = df.keywords[df.keywords == "unknown"].apply(lambda x: [])

    cat_features = ['genres', 'directors', 'filming_locations', 'keywords']

    for feature in cat_features:
       df = df.drop(feature, axis=1).join(df[feature].str.join('|').str.get_dummies())

            

    df_train, df_test = df[:train_size], df[train_size:]
    df_test.index = np.arange(len(df_test))

    regressor = CatBoostRegressor(n_estimators=900, max_depth=9, logging_level='Silent', cat_features=categ_features, allow_writing_files = False)    
    regressor.fit(df_train, y_train)
    return regressor.predict(df_test)
