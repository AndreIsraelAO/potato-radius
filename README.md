# Potato Radius Tool

Ferramenta simples em C para calcular o **Potato Radius**, isto é, o raio mínimo em que um corpo astronômico deixa de ser predominantemente irregular (“batata”) e torna-se aproximadamente esférico devido ao equilíbrio entre **gravidade própria** e **resistência mecânica do material**.  
A implementação utiliza a formulação apresentada por Lineweaver & Norman (2010), permitindo ao usuário inserir **yield strength** (σᵧ) e **densidade** (ρ) para qualquer material.

---

## Conceito

Corpos pequenos no Sistema Solar, como asteroides e cometas, não possuem gravidade suficiente para superar a resistência estrutural de seu próprio material.  
Quando o corpo atinge um raio mínimo — o **Potato Radius** — sua gravidade passa a dominar a resistência mecânica, levando-o a um formato mais esférico.

A expressão geral aproximada utilizada é:

\[
R \approx \sqrt{\frac{2\,\sigma_{y}}{\pi\,G\,\rho^{2}}}
\]

Onde:  
- **R** = Potato Radius (m)  
- **σᵧ** = Compressive yield strength do material (Pa)  
- **ρ** = densidade (kg/m³)  
- **G** = constante gravitacional (6.674×10⁻¹¹ m³/kg/s²)

Essa formulação permite experimentar com materiais rochosos, metálicos, gelados ou hipotéticos.

---

## Funcionalidades

- Entrada interativa de:
  - **Yield strength (Pa)**
  - **Densidade (kg/m³)**
- Cálculo direto do Potato Radius usando a equação física.
- Função própria de **raiz quadrada via Método de Newton**, evitando dependência de bibliotecas externas.
- Limpeza de tela compatível com Windows e Linux.
- Loop contínuo para múltiplos cálculos.

---

## Como usar

### Compilação

```bash
gcc potato_radius.c -o potato

