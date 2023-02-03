module Consts where

import Graphics.Gloss.Interface.Pure.Game

-- Game grid size: board size = gridSize x gridSize
gridSize :: Int
gridSize = 8

-- Center of a board grid
boardCentre :: Int
boardCentre = gridSize `div` 2 - 1

-- Starting score in a game
startScore :: (Int, Int)
startScore = (2, 2)

-- Name of the game - Reversi
gameName :: String
gameName = "Reversi"

-- Game window size
windowSizeX :: Int
windowSizeX = 580
windowSizeY :: Int
windowSizeY = 700

-- Offset relative to the center to the lower left corner of the window
lowerLeftCornerX :: Int
lowerLeftCornerX = - (windowSizeX `div` 2)
lowerLeftCornerY :: Int
lowerLeftCornerY = - (windowSizeY `div` 2)

-- Offset of grid to the lower left corner
gridAbsoluteLeftX :: Int
gridAbsoluteLeftX = 50 -- From left edge
gridAbsoluteLeftY :: Int
gridAbsoluteLeftY = 70 -- From bottom

-- Position of a game window on a screen
windowPositionX :: Int
windowPositionX = 450
windowPositionY :: Int
windowPositionY = 80

-- Board square size
squareSize :: Int
squareSize = 60

-- Radius of game disk
circleSize :: Float
circleSize = 25

-- Radius of disk of score
scoreCircleSize :: Float
scoreCircleSize = 35

-- Position of current player turn text
turnTextPositionX :: Float
turnTextPositionX = 50
turnTextPositionY :: Float
turnTextPositionY = 100

-- Position of white player score: disk and text
whiteScoreDiskPosX :: Float
whiteScoreDiskPosX = 80
whiteScoreDiskPosY :: Float
whiteScoreDiskPosY = 620
whiteScoreTextPosX :: Float
whiteScoreTextPosX = 175
whiteScoreTextPosY :: Float
whiteScoreTextPosY = 835

-- Position of black player score: disk and text
blackScoreDiskPosX :: Float
blackScoreDiskPosX = 400
blackScoreDiskPosY :: Float
blackScoreDiskPosY = 620
blackScoreTextPosX :: Float
blackScoreTextPosX = 635
blackScoreTextPosY :: Float
blackScoreTextPosY = 835

-- Game Over text
gameOverText :: String
gameOverText = "GAME OVER"

-- Position of Game Over text
gameOverTextPositionX :: Float
gameOverTextPositionX = 95
gameOverTextPositionY :: Float
gameOverTextPositionY = fromIntegral $ - lowerLeftCornerY + 50

-- Winner text position
winnerTextPositionX :: Float
winnerTextPositionX = 128
winnerTextPositionY :: Float
winnerTextPositionY = fromIntegral $ - lowerLeftCornerY - 50

-- Draw text position
drawTextPositionX :: Float
drawTextPositionX = 218
drawTextPositionY :: Float
drawTextPositionY = fromIntegral $ - lowerLeftCornerY - 50

-- Try Again text
tryAgainText :: String
tryAgainText = "Press SPACE to try again"

-- Position of Try Again text
tryAgainTextPositionX :: Float
tryAgainTextPositionX = 30
tryAgainTextPositionY :: Float
tryAgainTextPositionY = fromIntegral $ - lowerLeftCornerY - 150

-- Game display mode
display :: Display
display = (InWindow gameName (windowSizeX, windowSizeY) 
                             (windowPositionX, windowPositionY))
-- Board color
boardColor :: Color
boardColor = makeColorI 20 180 15 255  -- dark green

-- Board grid color
gridColor :: Color
gridColor = white

-- All game texts color except Game Over text
textColor :: Color
textColor = white

gameOverTextColor :: Color
gameOverTextColor = black

-- Background color for Game Over
bgColor :: Color
bgColor = makeColorI 128 128 128 200

-- Simulation steps per second
fps :: Int
fps = 60
