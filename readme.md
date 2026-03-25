---
title: "R1: Demoscene"
subtitle: "Fi del Món."
author: "Iris Garcia Gomez"
date: "25 de març de 2026"
geometry: margin=2.5cm
lang: ca
---
# Introducció

## Aspectes tèncins
L'arquitectura d'aquest projecte s'ha construit sobre **C++** i les llibreries **SDL2 (Simple DirectMedia Layer)**, **SDL2\_image** i **SDL2\_mixer**.

Un dels aspectes més importants a destacar d'aquest projecte és la **manipulació directa de buffers de memòria** en lloc 
d'utilitzar el `SDL_Renderer`. S'ha optat per accedir directament a les estructures de superfície (`SDL_Surface`) per controlar
renderitzat píxel a píxel amb algorismes. Aquest control es produeix a nivell de **CPU**, enlloc d'utilitzar *shaders* que pasen per la targeta gràfica.

# Disseny

## Color
A aquesta Demo, he prioritzat mantindre una paleta de colors cohesionada i coherent en tots els efectes. Així el color serveix
com a fil conductor narratiu i visual de l'escena.

Durant l'estudi dels apunts i dels exemples presentats, un dels aspectes que més em va cridar l'atenció va ser 
la falta de cohesió cromàtica dels exemples. Així m'he interessat per canviar és el control algorítmic del
color i construir paletes de colors cromàtiques. Per resoldre-ho he implementat la classe estàtica `PaletteBuilder`, encarregada de generar paletes de colors cromàtiques. 

**Implementació de `PaletteBuilder`:**

La classe utilitza un algorisme d'interpolació lineal en l'espai RGB per generar un gradient de colors. Així el mètode
`buildLinearPalette` rep dos colors RGB i el paràmetre $n$ (nombre de colors de la paleta final) i retorna un vector amb els colors.


# Timeline
// TODO: Explicar `Timeline`

# Musica 

# Efectes

# Transicions
Les transicions estan controlades pel TransitionManager. Les transicions reben 3 surfaces: Screen (`screen`) i els dos
efectes (`src1` i `src2`) i un paràmetre $\alpha \in [0,1]$, que mesura el progrés de transició. Així doncs les transicions modifiquen 
els buffers dels efectes seleccionant quins pixels es copien al buffer de la pantalla.

S'han implementat 4 tipus de transició:

* **Alpha blend:** Realitza una interpolació lineal entre els canals RGB d'ambdues superfícies. És una transició on el 
color final $C$ es calcula com: $C = (1-\alpha)C_1 + \alpha C_2$.

Les següents transicions estan basades en una condició de llindar (*threshold*) en funció d'una distància que depen de $x$ i de $y$:
$$\Delta(x, y) < \alpha$$

* **Diagonal 1:** Un front de tall que avança des del cantó superior esquerre. Utilitza la fórmula de la recta:
  $$\Delta(x, y) = \frac{x + y}{\text{width} + \text{height}} < \alpha \text{.}$$

* **Diagonal 2:** Inversa de l'anterior:
  $$\Delta(x, y) = \frac{(\text{width} - x) + y}{\text{width} + \text{height}} <\alpha \text{.}$$

* **Diffuse Ellipse:** Una el·lipse que neix del centre i s'expandeix cap a la vora de la pantalla. Utilitza una funció
de distància el·líptica amb un component de soroll a la vora:
$$\Delta(x,y) = \frac{(x - x_c)^2}{a^2} + \frac{(y - y_c)^2}{b^2} + \xi < \alpha \text{,}$$ on $x_c$ i $y_c$ fan 
refèrencia la posició del centre, $\xi$ és un component de soroll i $a, b$ són els semieixos de l'el·lipse i canvien amb 
funció del temps.
