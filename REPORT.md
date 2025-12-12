# REPORT

Polityka overflow: domyślnie odrzucamy nowe bajty (drop new). Przy przepełnieniu inkrementujemy licznik `dropped` i dodatkowo w `shell` zliczamy `broken_lines` — przypadki, gdy fragment komendy został odcięty przez overflow.

Konsekwencje: prostsza implementacja, mniejsze przesunięcia danych, ale utrata danych wejściowych i konieczność detekcji uciętych linii po stronie parsera.

Przykładowy fragment logu (wyjście programu):

```
READY
set=0.000 ticks=1 drop=0 broken=0
OK set=0.420
rx_free=126 tx_free=127 rx_count=1
ECHO hello world
ERR
... (wiele ERR od noop przy przepełnieniu)
set=0.420 ticks=... drop=... broken=...
```

Wnioski: licznik `dropped` informuje o skali utraty bajtów, `broken_lines` pomaga wykryć ucięte komendy. Można rozważyć alternatywną politykę "drop oldest" aby zawsze zachować najnowsze dane, kosztem przesuwania wskaźników i potencjalnie większej złożoności.
