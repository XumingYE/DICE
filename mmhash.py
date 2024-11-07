import mmh3
hasher = mmh3.mmh3_x64_128(seed=42)
hasher.update(b"foo bar code")

print(hasher.uintdigest())
