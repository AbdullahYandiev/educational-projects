module Board where

import Types
import Consts

import Data.Array

-- Define an initial state of a board
initBoard :: Board
initBoard = array 
  ((0, 0), (gridSize-1, gridSize-1)) 
  (mapBoardSquares $ \square -> (square, initSquareValues square))

-- Apply function to all indices of a board grid
mapBoardSquares :: (Coordinate -> a) -> [a]
mapBoardSquares f = [f (x, y) | x <- [0..gridSize-1], y <- [0..gridSize-1]]

-- Initialize values, where required by the rules
initSquareValues :: Coordinate -> BoardSquare
initSquareValues coordinate
  | coordinate == (boardCentre, boardCentre)         = Just White
  | coordinate == (boardCentre + 1, boardCentre + 1) = Just White
  | coordinate == (boardCentre, boardCentre + 1)     = Just Black
  | coordinate == (boardCentre + 1, boardCentre)     = Just Black
  | otherwise = Nothing

-- Function that checks if a given coordinate is within the given board
coordinateInBounds :: Board -> Coordinate -> Bool
coordinateInBounds board coordinate = inRange (bounds board) coordinate

-- Function that gets a disk from a coordinate
diskAtCoordinate :: Board -> Coordinate -> BoardSquare
diskAtCoordinate board coordinate = board ! coordinate

-- Definition of the opposing player for a given player
opposingPlayer :: Player -> Player
opposingPlayer White = Black
opposingPlayer _ = White
