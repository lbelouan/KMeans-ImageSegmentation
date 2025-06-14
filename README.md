# Kmeans - Algorithme de Segmentation et Classification d'Image

## Avantages et Inconvénients

### Avantages
- Sa mise en œuvre est relativement simple.
- Plus rapide que meanshift.

### Inconvénients
- Nombre de classes à déterminer.
- Convergence pas obligatoire.

---

## Principe de fonctionnement

L’algorithme **K-means** est un algorithme de classification qui regroupe les pixels en **N classes** en fonction de leur **valeur de couleur**, sans prendre en compte leur position spatiale.

1. **Initialisation des centres**  
   - Définir **N** clusters.  
   - Sélectionner **aléatoirement** N centres dans l’espace des couleurs.

2. **Assignation des pixels aux clusters**  
   - Pour chaque pixel de l’image, **trouver le centre \( Ck \) le plus proche**, en utilisant une **distance colorimétrique** (ici: distance euclidienne dans l’espace RGB).  
   - Attribuer au pixel l’étiquette du cluster correspondant.

3. **Mise à jour des centres des clusters**  
   - Recalculer chaque centre \( Ck \) en prenant la **moyenne des couleurs** des pixels qui lui sont assignés.

4. **Convergence**  
   - Répéter les étapes **2 et 3** jusqu’à ce que les centres de clusters ne changent plus ou que le nombre d’itérations maximal soit atteint.

## Points d'attention

- **Choix du nombre de clusters \( N \)** :  
  - Un **\( N \) trop petit** peut ne pas capturer suffisamment de variations de couleur.  
  - Un **\( N \) trop grand** peut entraîner une segmentation trop fine et du bruit.

- **Critère de convergence** :  
  - L’algorithme s’arrête lorsque **les centres ne changent plus de manière significative** ou après un **nombre fixe d’itérations**.

## Sensibilité à l'initialisation

L'initialisation des centres influence fortement le résultat final de l'algorithme. Pour éviter une convergence vers un mauvais minimum local, nous avons utilisé une **stratégie basée sur l'histogramme du canal bleu** (ligne 187 du code).

### Étape 1 : Construction de l'histogramme

Nous construisons un **histogramme des intensités du canal bleu** de l’image, qui nous permet de voir la distribution des valeurs entre `0` et `255`.

| Exemple d’histogramme sur un canal gris|
|-----------------------------------------|
| ![Histogramme du canal bleu](https://github.com/user-attachments/assets/1fba2c0d-8577-4da2-a4f1-5becd6962a30) |

Le but est d’**extraire les pics principaux** de cet histogramme, qui correspondent aux couleurs dominantes de l’image.

Avec la fonction **calcHist** on calcule l'histogramme puis avec **minMaxLoc** on trouve le premier max.

Pour le deuxième, on parcours l'histogramme pour le trouver en regardant les valeurs de **maxVal2** qui correspond au nombre de pixel d'une couleur d'indice **maxIdx2**.

On vérifie que les deux centres ne soit pas trop proche pour bien polariser les couleurs. 

---

## Exemples d'Utilisation sur des Images de Texture (512 × 512) avec K = 2

|Référence|Segmentation avec K-means open CV|Segmentation avec mon K-means|
|-----------|------------------------|------------------------|
| ![image](https://github.com/user-attachments/assets/624e8799-176a-4a6c-a8cf-684e80aa7ade) | ![image](https://github.com/user-attachments/assets/b56bdfae-363a-41ab-a47d-ff0e06315756)| ![image](https://github.com/user-attachments/assets/41e48772-6644-4343-947d-2f7c7caddf18)|

---

## Résultats

| Métrique | OpenCV | Implémentation personnelle |
|----------|--------|---------------------------|
| **Précision (`Pmine`)** | 0.885051 | 0.885051 |
| **Sensibilité (`Smine`)** | 0.987463 | 0.987463 |
| **Dice Similarity Coefficient (`DSCmine`)** | 0.933456 | 0.933456 |
| **Temps d'exécution** | **1.17036 s** | **1.68803 s** |

**On remarque que les deux solutions sont très proches mais l'implémentation du kmeans d'OpenCv est plus simple et cette solution est un peu plus rapide**


---

|Référence|Segmentation avec K-means open CV|Segmentation avec mon K-means|
|-----------|------------------------|------------------------|
|![image](https://github.com/user-attachments/assets/7ad5353e-7549-4f3f-ba4a-527a7abba41c)| ![image](https://github.com/user-attachments/assets/7dddd4a7-a265-4266-855c-d574aa5fe414)| ![image](https://github.com/user-attachments/assets/97af9cf7-2467-4556-ab1f-331b29ae3b2e)|

---

## Résultats

| Métrique | OpenCV | Implémentation personnelle |
|----------|--------|---------------------------|
| **Précision (`Pmine`)** | 0.357869 | 0.87276 |
| **Sensibilité (`Smine`)** | 0.894387 | 0.909796 |
| **Dice Similarity Coefficient (`DSCmine`)** | **0.511195** | **0.890893** |
| **Temps d'exécution** | 1.4648 | 1.45371 |

**On remarque que dans certains cas le choix des centres si il n'est pas fait intelligemment rend faux toute la segmentation. Comme ici ou notre code devient bien meilleur.**


---

## Sources

- **Cours ENSEEIHT - Sylvie Chambon**
