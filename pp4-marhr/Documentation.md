Mariana R. Hernández Rocha

CU. 150845

# Diseño del compilador
## Documentación PP4

### Archivos iniciales
La base tomada para esta nueva entrega fueron los archivos del repositorio de GitHub que se proveyó. Esto debido a que el diseño anterior aún tenía muchas dificultades para reconocer algunos errores semánticos.

Esta decisión motivó a el uso de métodos puramente virtuales que nos permitieran implementar los métodos *Check()* en lugar de validar para cada subclase, si era posible *castear*.

La implementación del método *Check()* en la clase *Stmt* hace posible que la evaluación en curso, ejecute el método de la subclase correspondiente. Este método se extiende a las subclases de los diferentes *statements* pero también a la clase *Expr*.

### Expresiones

Para que el compilador fuera capaz de arrojar errores de **compatibilidad de tipos entre operandos**, se aprovechó el uso de métodos puramente virtuales.

Declarar el método *GetType()* en la clase *Expr* hace posible que las subclases comparen el Tipo al que pertenecen sus elementos. 

Las subclases de la categoría Constantes (*intConstat*,*doubleConstant*,...) son las que cortan el proceso de obtener "recursivamente" el tipo de los elementos en la expresión al regresar valores de la clase *Type*.

Las reglas que involucran identificadores, implican que el método *GetType* de estas clases sea diferente, pues es necesario buscar el identificador dentro del *scope* para obtener su Tipo y validar la expresión. La estrategia en este caso, es buscar la declaración de dicho identificador y en algunas ocasiones, *castear* esa declaración para poder acceder a más información.

En cuanto a los **arreglos**, la detección de dimensiones enteras para los arreglos se hace dentor de la función *Check()* de la clase *NewArrayExpr* al comparar el tipo del atributo *size* con la constante entera.

Mientras que dentro de la función *Check* de la clase *ArrayAccess* es donde se validan dos condiciones, que los parétensis cuadrados se usen de manera correcta, es decir, sólo para acceder a arreglos y que los índices del arreglo sean valores enteros.

Para la clase **This** únicamente se validó que la expresión se encontrara dentro del contexto de una clase, obteniendo la clase de la instancia que se está analizando y *casteándola* con la clase *CLassDecl*.

La clase *FieldAccess*, es la más complicada de obtener el tipo y comprobar su validez. Usando el atributo *field* es posible buscar la declaración y así conocer el tipo que le corresponde. Para checar que la expresión es correcta, depende de si hay base, se obtiene su declaración y se evalúa si existe o no el identificador, si corresponde al base o si no es posible acceder a ese fied.

La clase *Call* también dependerá de si existe una variable base, se obtiene la declaración correspondiente y su tipo. Para corrobar que sea  una expresión correcta se creó el método *ValidateActuals* que evalúa que haya la misma cantidad de argumentos a los que se espera y que sean del mismo tipo.

### Declaraciones

Las únicas modificaciones que se hicieron en los archivos de declaraciones, fué cambiar la visibilidad de algunos atributos para ciertas clases. Esto para poder acceder a sus valores (por ejemplo de tipos) desde las expresiones.

### Statements

Se modificaron las clases de statements condicionales para validar que los tipos usados fueran booleanos. Y en el caso del *break*, que sólo se llamara dentro de *loops*. 

