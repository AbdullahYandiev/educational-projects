module Moves where

import Types
import Board

import Data.Array
import Data.Maybe

-- Returns a list of coordinates that should be changed to the player's
-- color on a board after clicking on some square
getMovesOnClick :: Board -> Player -> Coordinate -> [Coordinate]
getMovesOnClick board player baseCoordinate
  -- Not a valid base coordinate -> not valid move
  | not isValidBaseCoordinate = []
  -- Not an empty square -> not valid move
  | isJust baseSquare = []
  -- No valid directions -> not valid move
  | null resultDirections = []
  -- Valid move -> include our base coord at the top and return a list
  | otherwise = baseCoordinate : resultDirections
    where
      isValidBaseCoordinate = coordinateInBounds board baseCoordinate
      baseSquare = diskAtCoordinate board baseCoordinate
      resultDirections = concat $ map findSquaresForDisks directionsToCheck
      directionsToCheck = [(-1,-1), (-1,0), (-1,1), (0,-1), (0,1), (1,-1), (1,0), (1,1)]
      (baseX, baseY) = baseCoordinate

      -- Function that walks in a given direction and looks for new squares for player disks
      -- If it terminates on a placed opponent square return the resulting list, else empty list
      findSquaresForDisks :: Coordinate -> [Coordinate]
      findSquaresForDisks direction = accumeSquares (baseX+dirX, baseY+dirY) direction []
        where
          (dirX, dirY) = direction

      -- Helper function that accumulates new squares
      accumeSquares :: Coordinate -> Coordinate -> [Coordinate] -> [Coordinate]
      accumeSquares currentSquare direction accumulatedSquares
        | isLastSquare = accumulatedSquares
        | isValidSquare = 
            accumeSquares (curX+dirX, curY+dirY) direction $ currentSquare : accumulatedSquares
        | otherwise = []
        where
          (curX, curY) = currentSquare
          (dirX, dirY) = direction
          isLastSquare = isValidPosition && (currentDisk == (Just player))
          isValidSquare = isValidPosition && (currentDisk == (Just opponent))
          isValidPosition = coordinateInBounds board currentSquare
          currentDisk = diskAtCoordinate board currentSquare
          opponent = opposingPlayer player

-- Returns a list containing lists of coordinates for applying moves 
-- The first item in each list is clicked by a given player square 
movesAvailableForPlayer :: Board -> Player -> [[Coordinate]]
movesAvailableForPlayer board player = filter (not . null) mappedCoordinates
  where
    mappedCoordinates = mapBoardSquares (getMovesOnClick board player)

-- Determines if no moves are possible at all on a given board
noMovesAvailableForBoth :: Board -> Bool
noMovesAvailableForBoth board = (null whiteAvailableMoves) && (null blackAvailableMoves)
  where
    whiteAvailableMoves = movesAvailableForPlayer board White
    blackAvailableMoves = movesAvailableForPlayer board Black

-- Applies a move: changes color of desired board squares to the color of a given player
applyMove :: Board -> Player -> [Coordinate] -> Board
applyMove board player disksList = board // newDisksList
  where
    newDisksList = map (\coordinate -> (coordinate, Just player)) disksList
