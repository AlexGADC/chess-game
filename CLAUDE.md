# Chess Game - instructiuni pentru Claude

Proiect de echipa (4 studenti) pentru un curs: joc de sah in C++.
Deadline: 6 decembrie 2026, ora 23:00. Prezentare: 9 decembrie 2026.

## Structura (Visual Studio, o singura solutie .sln)

- `ChessLogic/` - logica jocului, Static Library.
  - `ChessLogic/API/` - singura parte publica: interfete (`IGame`, `IGameListener`, `IPieceInfo`),
    enum-uri (`EColor`, `EPieceType`, `EGameState`), `Position`, exceptiile din `ChessExceptions.h`.
  - restul (`Game`, `Board`, `Piece` si piesele) sunt clase interne.
- `ChessUI/` - interfata grafica. Include DOAR headere din `ChessLogic/API/`.
- `ChessTests/` - unit teste cu GTest + GMock.

## Reguli obligatorii (sunt punctate la evaluare)

1. **Nu modifica nimic din `ChessLogic/API/` fara sa ma intrebi.** API-ul e contractul dintre colegi;
   o schimbare acolo strica munca celorlalti. Daca crezi ca e nevoie, propune schimbarea si opreste-te.
2. UI-ul nu contine logica jocului si nu include `Game.h`, `Board.h`, `Piece.h`.
3. Design pattern Observer: `Game` e subiectul, `IGameListener` e observatorul.
   Toate notificarile trec prin `Game::NotifyListeners()`.
4. Fara cod duplicat: extrage functii comune (ex. `Piece::GetMovesInDirections()`).
5. Nume sugestive, in engleza, pentru clase, metode si variabile.
6. Orice schimbare in logica vine cu unit teste. Testele trebuie sa pice daca logica devine gresita
   (verifica valori concrete, nu doar ca "nu arunca").
7. Mock-urile pentru interfete se fac cu GMock (`MOCK_METHOD`).

## Conventii de cod

- C++17, `#pragma once`, `enum class`, tab-uri pentru indentare.
- Membrii privati: prefix `m_` (ex. `m_currentPlayer`).
- Pointeri: `std::shared_ptr` pentru obiectele expuse, `std::weak_ptr` pentru listeneri.
- Erorile se semnaleaza cu exceptiile din `ChessExceptions.h`, nu cu coduri de return.
- Coordonate: `row 0` = linia 1 (randul de baza al albului), `col 0` = coloana a.

## Git

- Nu lucra pe `main`. Fiecare coleg are branch-ul lui; intra pe `main` doar prin pull request.
- Nu face commit sau push fara sa ma intrebi.

## Build si teste

- Compilarea si testele se ruleaza din Visual Studio (Build Solution, apoi Test Explorer).
- Daca nu poti rula build-ul, spune-mi si iti trimit eu erorile.
