"""
Generate all the possible keys, to feed into a dictionary
"""
for i in range(2**24):
    low_byte = i & 0xff
    middle_byte = (i >> 8) & 0xff
    high_byte = (i >> 16) & 0xff
    print(f"51{high_byte:02x}{middle_byte:02x}10{low_byte:02x}0a")
    
    
