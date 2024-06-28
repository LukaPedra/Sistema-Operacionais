import random

def generate_access_file(filename, num_lines=100):
    with open(filename, 'w') as f:
        for _ in range(num_lines):
            page_number = random.randint(0, 15)  # Páginas de 0 a 15
            access_type = random.choice(['R', 'W'])  # Leitura ou Escrita
            f.write(f"{page_number:02d} {access_type}\n")

# Gerar arquivos para P1, P2 e P3
for i in range(1, 4):
    generate_access_file(f"acessos_P{i}")

print("Arquivos de acesso gerados com sucesso.")