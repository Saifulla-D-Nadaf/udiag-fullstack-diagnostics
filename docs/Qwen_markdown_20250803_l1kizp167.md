# µDiag Communication Protocol

## UART Frame
- Baud: 115200
- Format: 8N1
- Data: 1 byte fault flags

## Fault Flag Encoding
| Bit | Meaning           |
|-----|-------------------|
| 0   | Temp > 130°C      |
| 1   | Temp < 20°C       |
| 2   | Volt > 11.0V      |
| 3   | Volt < 9.0V       |
| 4-7 | Reserved (0)      |

## Cloud JSON Schema
```json
{
  "flags": 5,
  "ts": 1712345678901
}