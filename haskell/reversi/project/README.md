# Реверси

**Постановка задачи.**  
Игра "Реверси" на языке Haskell. При запуске открывается игровое поле, аналогичное оригинальному игровому полю настолько игры. Доступен лишь формат игры "Человек против человека". Допустимость ходов определяется по оригинальным правилам игры. Очередность ходов указана в нижнем левом углу. Для осуществления хода достаточно лишь нажать на соответствующую клетку поля. Если нажать на недоступную клетку или недоступный участнок игрового окна, то ничего не происходит. В течение всей игры участникам отображается текущий счет. В конце игры отображается победитель или надпись о ничейном результате. Для начала новой игры необходимо нажать клавишу пробела. Для завершения игровой сессии - клавишу "Esc".

**Модули, на которые разделен проект:**  

*Main:* Корневой модуль, запускающий игру.
*Функция:* main.

*Game:* Основной модуль, собирающий все данные и запускающий процесс игры при помощи функции play из библиотеки Graphics.Gloss.Interface.Pure.Game.
*Функция:* runGame.

*InitGame:* Модуль, отвечающий за инициализацию игры.
*Функция:* initGame.

*Events:* Модуль, отвечающий за обновляющие состояние игрового поля в зависимости от нажатий клавиш игроками.
*Функции:* handleEvent, updateGame.

*Draw:* Модуль, отвечающий за построения интерфейса игры. 
*Функция:* drawGame.

*Moves:* Модуль, отвечающие за применение ходов к игровому полю.
*Функции:* getMovesOnClick, movesAvailableForPlayer, noMovesAvailableForBoth, applyMove.

*Board:* Модуль, отвечающий за логику построения и обновления игрового поля.
*Функции:* initBoard, mapBoardSquares, coordinateInBounds, diskAtCoordinate, opposingPlayer.

*Types:* Модуль, содержащий все дополнительно объявленные типы данных.
*Типы:* GameWorld, Board, Player, Score.

*Consts:*
Модуль, содержащий все объявленные в коде программы константы.

**Используемые библиотеки.**  
Gloss, Array, Maybe.

**Работа с приложением.**  
Нажатие левой кнопкой мыши на соответствующую клетку игрового поля для осуществления хода.
Начатие пробела в конце игры для начала новой игры.
Для запуска надо перейти в корень папки проекта и запустить приложение с помощью stack run.

