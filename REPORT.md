# REPORT

## Polityka Overflow

Domyślna polityka: **drop new bytes** — odrzucamy nowe bajty gdy bufor jest pełny.

- Ring buffer (`rb_t`) śledzi `dropped` — liczbę utraconych bajtów
- `dropped` inkrementuje się w `rb_put()` gdy jest przepełnienie
- Konsekwencje: prosta implementacja, brak przesuwania wskaźników, ale utrata danych wejściowych

## Detekcja Przerwanych Linii (Option B)

**Problem z naiwnym podejściem:** Liczenie `broken_lines` za każdy odrzucony bajt w `shell_rx_bytes()` powoduje, że licznik rośnie do 1073 dla jednego zdarzenia overflow (1200 bajtów → 1073 odrzucone).

**Rozwiązanie:** Przenieść detekcję do `shell_tick()`, gdzie śledzimy zdarzenia overflow podczas montażu linii komend:

1. Statyczna zmienna `prev_dropped` przechowuje poprzednią wartość `rx.dropped`
2. W `shell_tick()` każdy tick:
   - Jeśli `rx.dropped > prev_dropped` AND `n > 0` (linia w trakcie przetwarzania):
     - Komenda została przerwana przez overflow
     - Inkrementuj `broken_lines` dokładnie raz
   - Istniejąca logika obsługuje przepełnienie bufora linii (128 bajtów)

## Test: Partial Command + Overflow Burst

Scenario: wysyłamy niekompletną komendę, a potem burst, który powoduje overflow:

```
=== Test 1: Basic commands ===
READY
set=0.000 ticks=1 drop=0 broken=0
OK set=0.420
rx_free=126 tx_free=127 rx_count=1
ECHO hello world

=== Test 2: Partial command + burst (should break line) ===
OK set=1.500
ERR
ERR
... (20 × ERR z fragmen­tów w buforze)
After burst: dropped=1073, broken=1
ERR

Final: dropped=1073, broken=1
```

Wyjaśnienie:
- Wysłano `"set 1.5"` (niekompletnie) → zachowuje się w RX
- Wysłano 200 × `"noop\r\n"` (1200 bajtów) → 1073 bajty odrzucone
- `broken=1` (nie 1073!) — dokładnie jedna komenda przerwana
- Fragmenty `"noop"` w buforze zwracają ERR

## Wnioski

1. **`dropped` licznik:** Informuje o skali utraty bajtów na poziomie ring buffera
2. **`broken_lines` licznik:** Odzwierciedla rzeczywiste zdarzenia przerwania komend, a nie liczbę bajtów
3. **Detekcja na poziomie parsera:** Lepsze zrozumienie semantyki — overflow w kontekście montażu komend
4. **Alternatywa "drop oldest":** Byłaby bardziej złożona (przesuwanie wskaźników) ale zawsze przechowałaby najnowsze dane
