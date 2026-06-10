curl -s wttr.in/"Calafate" | awk '/°C/ {for (i=1; i<=NF; i++) if ($i ~ /°C/) print $(i-1) " " $i; exit}'

