import sys

def decode_dw1000_status(hex_value):
    """
    Decode DW1000 System Event Status Register
    """

    if not hex_value.startswith('0x'):
        raise ValueError("Hex value must start with '0x'")
    status_reg = int(hex_value, 16)
    
    status_bits = {
        0: ("IRQS", "Interrupt Request Status"),
        1: ("CPLOCK", "Clock PLL Lock"),
        2: ("ESYNCR", "External Sync Clock Reset"),
        3: ("AAT", "Automatic Acknowledge Trigger"),
        4: ("TXFRB", "Transmit Frame Begins"),
        5: ("TXPRS", "Transmit Preamble Sent"),
        6: ("TXPHS", "Transmit PHY Header Sent"),
        7: ("TXFRS", "Transmit Frame Sent"),
        8: ("RXPRD", "Receiver Preamble Detected"),
        9: ("RXSFDD", "Receiver Start Frame Delimiter Detected"),
        10: ("LDEDONE", "LDE processing done"),
        11: ("RXPHD", "Receiver PHY Header Detect"),
        12: ("RXPHE", "Receiver PHY Header Error"),
        13: ("RXDFR", "Receiver Data Frame Ready"),
        14: ("RXFCG", "Receiver FCS Good"),
        15: ("RXFCE", "Receiver FCS Error"),
        16: ("RXRFSL", "Receiver Reed Solomon Frame Sync Loss"),
        17: ("RXRFTO", "Receiver Frame Wait Timeout"),
        18: ("LDEERR", "Leading edge detection processing error"),
        20: ("RXOVRR", "Receiver Overrun"),
        21: ("RXPTO", "Preamble detection timeout"),
        22: ("GPIOIRQ", "GPIO interrupt"),
        23: ("SLP2INIT", "Sleep to Init"),
        24: ("RFPLL_LL", "RF PLL Losing Lock"),
        25: ("CLKPLL_LL", "Clock PLL Losing Lock"),
        26: ("RXSFDTO", "Receive SFD timeout"),
        27: ("HPDWARN", "Half Period Delay Warning"),
        28: ("TXBERR", "Transmit Buffer Error"),
        29: ("AFFREJ", "Automatic Frame Filter rejection"),
        30: ("HSRBP", "Host Side Receive Buffer Pointer"),
        31: ("ICRBP", "IC side Receive Buffer Pointer"),
        32: ("RXRSCS", "Receiver Reed-Solomon Correction Status"),
        33: ("RXPREJ", "Receiver Preamble Rejection"),
        34: ("TXPUTE", "Transmit Power Up Time Error"),
    }
    
    print(f"System Event Status Register: 0x{status_reg:08X}")
    print("=" * 80)
    print(f"{'Bit':<6}{'Field':<10}{'Description'}")
    print("-" * 80)
    
    for bit_pos in range(35):
        if (status_reg >> bit_pos) & 1:
            name, description = status_bits.get(bit_pos, ("-", "Reserved"))
            print(f"{bit_pos:<6d}{name:<10}{description}")

if __name__ == "__main__":
    
    if len(sys.argv) != 2:
        print("Usage: python decode_status.py <hex_value>")
        print("Example: python decode_status.py 0x12345678")
        sys.exit(1)
    
    hex_input = sys.argv[1]
    decode_dw1000_status(hex_input)