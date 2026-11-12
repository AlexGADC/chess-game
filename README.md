# Chess Game

Joc de sah in C++, impartit in 3 proiecte:

| Proiect | Ce contine |
|---|---|
| `ChessLogic` | Logica jocului (static library). Folderul `API/` e singura parte vizibila din afara. |
| `ChessUI` | Interfata grafica. Include **doar** headere din `ChessLogic/API/`. |
| `ChessTests` | Unit teste cu GTest + GMock. |

## API (`ChessLogic/API/`)

- Interfete: `IGame`, `IGameListener` (Observer), `IPieceInfo`
- Enum-uri: `EColor`, `EPieceType`, `EGameState`
- Elementare: `Position`, exceptiile din `ChessExceptions.h`

API-ul se schimba doar cu acordul intregii echipe.

## Conventie coordonate

`row 0` = linia 1 (randul de baza al albului), `col 0` = coloana a.

## Reguli Git

- Nu se lucreaza direct pe `main`: fiecare pe branch-ul lui, apoi pull request.
- Fiecare PR e verificat de colegul din aceeasi pereche.

## ChessUI este setat ca start up project!

- Am adaugat la proiectul ChessUI ca directories si libraries folderele include, respectiv lib din fisierul dependencies
  
