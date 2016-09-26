///////////////////PINES PARA LOS SENSORES ULTRASONICOS//////////////////////////
#define pulsoX 30//Pulso para el sensor en X
#define ecoX 31//Pin de respuesta sensor en X
#define pulsoY 32//Pulso para el sensor en Y
#define ecoY 33//Pin de respuesta sensor en Y
/////////////PINES PARA EL ACCIONAMIENTO DE LOS MOTORES X E Y////////////////////
#define enableX 34//Pin de enclavamiento del driver X
#define sentidoX 35//Pin de sentido del driver X
#define relojX 38//Pin señal de reloj del driver X
#define enableY 36//Pin de enclavamiento del driver Y
#define sentidoY 37//Pin de sentido del driver Y
#define relojY 39//Pin señal de reloj del driver Y
////////////////////////////PIN DEL ELECTROIMAN//////////////////////////////////
#define electro 40//Pin de salida del electroiman
///////////DECLARACION DE VARIABLES A UTILIZAR EN EL PROGRAMA////////////////////
int pwm = 255;//Valor pwm para el electroiman
String lectura = ""; //Variable de lectura del serial
boolean qx = false; //Bandera de funcionamiento eje X
boolean qy = false; //Bandera de funcionamiento eje Y
int captura; //}Variable con la indicacion de captura de pieza
int pix; //Valor de la columna de posicion inicial
int piy; //Valor de la fila de posicion inicial
int pfx; //Valor de la columna de posicion final
int pfy; //Valor de la fila de posicion final
int cb; //Valor de la columna de almacenamiento de blancas
int fb; //Valor de la fila de almacenamiento de blancas
int cn; //Valor de la columna de almacenamietno de negras
int fn;//Valor de la fila de almacenamiento de negras
String color;//Color de la pieza a mover
long casilla_X;//Valor de la casilla en X a llegar
long ax;//Valor del sensor ultrasonico X sensado
long casilla_Y;//Valor de la casilla en Y a llegar
long ay;//Valor del sensor ultrasonico Y sensado
int c = 0; //Variable tipo bandera para activar le electroiman
long error = 0.0; //Margen de error
long lm = 2.0; //Distancia entre el centro de la casilla y la orilla
long columna;//Valor de la casilla en X de almacenaje
long fila;//Valor de la fila en X de almacenaje
int pieza;//0=Peon,1=Torre,2=Caballo,3=Alfil,4=DamayRey
boolean bandera = false; //Bandera de seguridad
void setup() {
  ////////////////INICIALIZACION DE LOS PINES A UTILIZAR/////////////////////////
  pinMode(pulsoX, OUTPUT);
  pinMode(pulsoY, OUTPUT);
  pinMode(enableX, OUTPUT);
  pinMode(enableY, OUTPUT);
  pinMode(sentidoX, OUTPUT);
  pinMode(sentidoY, OUTPUT);
  pinMode(relojX, OUTPUT);
  pinMode(relojY, OUTPUT);
  pinMode(electro, OUTPUT);
  pinMode(ecoX, INPUT);
  pinMode(ecoY, INPUT);
  digitalWrite(enableX, HIGH);
  digitalWrite(enableY, HIGH);
  digitalWrite(sentidoX, LOW);
  digitalWrite(sentidoY, LOW);
  digitalWrite(pulsoX, LOW);
  digitalWrite(pulsoY, LOW);
  digitalWrite(electro, LOW);
  digitalWrite(pulsoX, LOW);
  digitalWrite(pulsoY, LOW);
  /////////////////INILIZACION DEL PUERTO SERIAL/////////////////////////////////
  Serial.begin(9600);
}
void loop() {
  lectura = "";
  while (lectura == "") {//Ciclo para esperar lectura de serial
    lectura = Serial.readString();
    int u = lectura.length(); //Determina el tamaño del vector recibido
    if (u != 11)//Si la cadena es mayor o menor de 11 limpa la lectura
      lectura = "";
  }
  ////////////DESCOMPONE LA CADENA EN LAS VARIABLES NECESARIAS///////////////////
  captura = lectura.substring(0, 1).toInt(); //Valor necesario 1 o 0
  pix = lectura.substring(1, 2).toInt(); //Valor de la columna 0 a 7
  piy = lectura.substring(2, 3).toInt(); //Valor de la fila 0 a 7
  pfx = lectura.substring(3, 4).toInt(); //Valor de la columna 0 a 7
  pfy = lectura.substring(4, 5).toInt(); //Valor de la fila 0 a 7
  cb = lectura.substring(5, 6).toInt(); //Valor de la columna blanca 0 a 1
  fb = lectura.substring(6, 7).toInt(); //Valor de la fila 0 a 7
  cn = lectura.substring(7, 8).toInt(); //Valor de la columna blanca 2 a 3
  fn = lectura.substring(8, 9).toInt(); //Valor de la fila 0 a 7
  color = lectura.substring(9, 10); //Color de la pieza b o n
  pieza = lectura.substring(10, 11).toInt(); //Numero de pieza
  if (captura == 1) {//Revisa si se captura una pieza
    //////////////Realiza los movimientos de X e Y para posicion final//////
    movimiento_X(pfx);
    movimiento_Y(pfy);
    c = 1;//Cambia valor de variable de electroiman
    while (c == 1) { //Movimiento de la pieza con electroiman
      electroiman(c);//Enciende el electroiman
      ax = sensorX();//Optiene el valor de sensor X
      casilla_X = casillaX(pfx); //Toma el valor de la casilla en donde esta
      if ((ax > casilla_X - lm)) //Mueve la pieza a la linea derecha
        motor_derecha();
      reloj_X();
      //determina si llego al lugar deceado
      if (ax > (casilla_X - lm - error) && ax < (casilla_X - lm + error)) {
        digitalWrite(enableX, HIGH);//Enclava el motor
        bandera = true;//Enciende bandera de seguridad
      }
      while (bandera == true) {
        qx = true;
        if (color.equals("b")) {
          /*Determina el color para saber a donde tiene que enviar
            la pieza a guardar*/
          columna = columnaX(cb);//Determina la posicion de la columna de almacenamiento
          fila = casillaY(fb);//Determina la posicion de la fila de almacenamiento
          movimiento_almacenaje_blancas(columna, fila);//Realiza el movimiento de almacenamiento
        } else if (color.equals("n")) {
          /*Determina el color para saber a donde tiene que enviar
              la pieza a guardar*/
          columna = columnaX(cn);//Determina la posicion de la columna de almacenamiento
          fila = casillaY(fn);//Determina la posicion de la fila de almacenamiento
          movimiento_almacenaje_negras(columna, fila);//Realiza el movimiento de almacenamiento
        }
        bandera = false;//Se apaga la bandera de seguridad
      }
    }
    ////////Se realiza el movimeinto a posicion inicial////////////
    movimiento_X(pix);//
    movimiento_Y(piy);
    c = 1;//Cambia valor de variable de electroiman
    electroiman(c);//Enciende el electroiman
    Movimiento_Pieza(pieza, captura);/*Se realiza el movimiento correspondiente
    de la pieza a mover*/
    c = 0;//Cambia valor de variable de electroiman
    electroiman(c);//Se apaga el electroiman
    Serial.write("Listo");//Se envia un String para saber cuando termino
  } else {
    ////////Se realiza el movimeinto a posicion inicial////////////
    movimiento_X(pix);
    movimiento_Y(piy);
    c = 1;//Cambia valor de variable de electroiman
    electroiman(c);//Enciende el electroiman
    Movimiento_Pieza(pieza, captura);/*Se realiza el movimiento correspondiente
    de la pieza a mover*/
    c = 0;//Cambia valor de variable de electroiman
    electroiman(c);//Se apaga el electroiman
    Serial.write("Listo");//Se envia un String para saber cuando termino
  }
}
///////////FUNCION PARA EL SENSADO DE LA DISTANCIA EN EL EJE X///////////////////
long sensorX() {
  long tiempo = 0;
  long distancia = 0;
  digitalWrite(pulsoX, LOW);//Se apaga el pin por 5 microsegundos para estabilidad
  delayMicroseconds(5);
  digitalWrite(pulsoX, HIGH);//Se enciende el pin
  delayMicroseconds(5);//SE hace una espera de 5 microsegundos
  tiempo = pulseIn(ecoX, HIGH);//Se cuenta el tiempo de espera en regreso el eco
  distancia = int(0.017 * tiempo);//Se realiza la operacion para conversion en CM
  return distancia;//Regresa el valor de la distancia
}
///////////FUNCION PARA EL SENSADO DE LA DISTANCIA EN EL EJE Y///////////////////
long sensorY() {
  long tiempo = 0;
  long distancia = 0;
  digitalWrite(pulsoY, LOW);//Se apaga el pin por 5 microsegundos para estabilidad
  delayMicroseconds(5);
  digitalWrite(ecoY, HIGH);//Se enciende el pin
  delayMicroseconds(5);//SE hace una espera de 5 microsegundos
  tiempo = pulseIn(ecoY, HIGH);//Se cuenta el tiempo de espera en regreso el eco
  distancia = int(0.017 * tiempo);//Se realiza la operacion para conversion en CM
  return distancia;//Regresa el valor de la distancia
}
////////////FUNCION PARA EL ACCIONAMIENTO DEL MOTOR X NEMA 17 ///////////////////
void motor_derecha() {
  digitalWrite(enableX, LOW);//Apaga el pin de enclavamiento
  digitalWrite(sentidoX, LOW);//Determina le sentido de giro
}
void motor_izquierda() {
  digitalWrite(enableX, LOW);//Apaga el pin de enclavamiento
  digitalWrite(sentidoX, HIGH);//Determina le sentido de giro
}
////////////FUNCION PARA EL ACCIONAMIENTO DEL MOTOR Y NEMA 23 ///////////////////
void motor_adelante() {
  digitalWrite(enableY, LOW);//Apaga el pin de enclavamiento
  digitalWrite(sentidoY, LOW);//Determina le sentido de giro
}
void motor_atras() {
  digitalWrite(enableY, LOW);//Apaga el pin de enclavamiento
  digitalWrite(sentidoY, HIGH);//Determina le sentido de giro
}
/////////FUNCION PARA LAS SEÑALES DE RELOJ DE AMBOS EJES/////////////////
void reloj_X() {//Eje X frecuencia tentativa 2KHz
  digitalWrite(relojX, HIGH);
  delayMicroseconds(500);
  digitalWrite(relojX, LOW);
  delayMicroseconds(500);
}
void reloj_Y() {//Eje Y frecuencia de 1.18KHz
  digitalWrite(relojY, HIGH);
  delayMicroseconds(850);
  digitalWrite(relojY, LOW);
  delayMicroseconds(850);
}
////////////////FUNCION PARA LA ACTIVACION DEL ELECTROIMAN///////////////////////
void electroiman(int x) {//Recive valor de la variable del electroiman
  if (x == 1)//Comprueba la variable del electroiman
    analogWrite(electro, pwm);//Enciende el electroiman al valor PWM
  else if (x == 0)//Comprueba la variable del electroiman
    digitalWrite(electro, LOW);//Apaga el electroiman
}
/////////////FUNCION PARA DETERMINAR LA DISTANCIA DE LA CASILLA X////////////////
long casillaX(int x) {
  /*Es necesario colocar las distancias que existen desde el extremo del tablero
    enfrente del sensor al centro de la casilla para poder tener los casos y el
    programa funcion con cualquier dimencion de tablero*/
  long y;
  switch (x) {
    case 0:
      y = 0.0;
      break;
    case 1:
      y = 0.0;
      break;
    case 2:
      y = 0.0;
      break;
    case 3:
      y = 0.0;
      break;
    case 4:
      y = 0.0;
      break;
    case 5:
      y = 0.0;
      break;
    case 6:
      y = 0.0;
      break;
    case 7:
      y = 0.0;
      break;
  }
  return y;
}
/////////////FUNCION PARA DETERMINAR LA DISTANCIA DE LA CASILLA Y////////////////
long casillaY(int x) {
  /*Es necesario colocar las distancias que existen desde el extremo del tablero
    enfrente del sensor al centro de la casilla para poder tener los casos y el
    programa funcion con cualquier dimencion de tablero*/
  long y;
  switch (x) {
    case 0:
      y = 0.0;
      break;
    case 1:
      y = 0.0;
      break;
    case 2:
      y = 0.0;
      break;
    case 3:
      y = 0.0;
      break;
    case 4:
      y = 0.0;
      break;
    case 5:
      y = 0.0;
      break;
    case 6:
      y = 0.0;
      break;
    case 7:
      y = 0.0;
      break;
  }
  return y;
}
///////FUNCION PARA DETERMINAR LA DISTANCIA DE LA COLUMNA DE ALMACENAMIENTO//////
long columnaX(int x) {
  /*Es necesario colocar las distancias que existen desde el extremo del tablero
    enfrente del sensor al centro de la casilla para poder tener los casos y el
    programa funcion con cualquier dimencion de tablero*/
  long y;
  switch (x) {
    case 0:
      y = 0.0;
      break;
    case 1:
      y = 0.0;
      break;
    case 2:
      y = 0.0;
      break;
    case 3:
      y = 0.0;
      break;
  }
  return y;
}
//////////FUNCION PARA REALIZAR EL MOVIMIENTO DE ALMACENAJE DE LA PIEZA/////////
void movimiento_almacenaje_blancas(long y, long z) {
  while (qx == true) {//Cillo para el movimiento del eje y para almacenar
    ay = sensorY();//Toma el valor del sensor Y
    casilla_Y = casillaY(pfy);//Determina la posicion de la fila
    if (casilla_Y > z) {//Determina si la casilla de destino esta antes
      if (ay < (casilla_Y + lm)) {//Se mueve a la linea superior
        motor_adelante();//Activa el motor hacia adelante
        reloj_Y();//Enciende la señal de reloj del eje Y
      }
      //Determina si ya llego a la posicion de line
      if (ay > (casilla_Y + lm - error) && ay < (casilla_Y + lm - error)) {
        digitalWrite(enableY, HIGH);//Enclava el motor Y
        while (true) {
          ax = sensorX();//Toma el valor del sensor X
          motor_derecha();//Enciende el motor X hacia la derecha
          if (ax > (y - error) && ax < (y - error)) {
            /*Determina si el motor
              llego a la columna deceado */
            digitalWrite(enableX, HIGH);//Enclava el motor X
            qx = false;//Enciende y apaga banderas de apoyo
            qy = true;
            break;//Rompe ciclo infinito
          }
          reloj_X();//Enciende la señal de reloj X
        }
        while (qy == true) {
          ay = sensorY();//Toma el valor del sensor Y
          motor_atras();//Enciende el motor X hacia atras
          if (ay < (z + error) && ay < (z - error)) {
            /*Determina ssi el motor
              llego a la fila deceada*/
            digitalWrite(enableY, HIGH);//Enclava el motor Y
            qy = false;//Apaga la bandera de apoyo
            c = 0;//Cambia la variable del electroiman
            electroiman(c);//Apaga el electroiman
          }
          reloj_Y();//Enciende la señal de reloj Y
        }
      }
    } else {
      if (ay < (z + lm)) {
        /*Activa el movimiento del motor
          Y hacia adelante para que llegue a la fila deceada
          mas la mitad de la casilla*/
        motor_adelante();//Enciende el motor Y hacia adelante
        reloj_Y();//Enciende señal de reloj Y
      }
      if (ay > (z - error) && ay < (z + error)) {
        /*Determina si llego
          a la posicion deseada*/
        digitalWrite(enableY, HIGH);//Enclava el motor Y
        while (true) {
          ax = sensorX();//Toma le valor del sensor X
          motor_derecha();//Enciende el motor X hacia la derecha
          if (ax > (y - error) && ax < (y + error)) {
            /*Determina si llego
              a la posicion deseada*/
            digitalWrite(enableX, HIGH);//Enclava el motor X
            qx = false;//Apaga y enciende banderas de apoyo
            qy = true;
            break;//Romple el ciclo
          }
          reloj_X();//Enciende la señal de reloj X
        }
        while (qy == true) {
          ay = sensorY();//Toma el valor del sensor Y
          motor_atras();//Enciende el motor Y hacia atras
          if (ay > (z - error) && ay < (z + error)) {
            /*Determian si llego
              a la posicion deseada*/
            digitalWrite(enableY, HIGH);//Enclava el motor Y
            qy = false;//Apaga bandera de apoyo
            c = 0;//Cambia variable de electroiman
            electroiman(c);//Apaga el electroiman
          }
          reloj_Y();//Enciende la señal de reloj Y
        }
      }
    }
  }
}
void movimiento_almacenaje_negras(long y, long z) {
  while (qx == true) {
    ay = sensorX();//Toma el valor del sensor X
    casilla_Y = casillaY(pfy);//Toma el valor de la fila
    if (casilla_Y > z) {//Determina si la casilla de destino esta antes
      if (ay < (casilla_Y + lm)) {//Se mueve a la linea superior
        motor_adelante();//Se activa el motor Y adelante
        reloj_Y();//Enciende señal de reloj Y
      }
      //Determina si llego a la posicion de la linea
      if (ay > (casilla_Y + lm - error) && ay < (casilla_Y + lm - error)) {
        while (true) {
          ax = sensorX();//Toma le valor del sensor X
          motor_izquierda();//Enciende el motor X hacia la izquierda
          reloj_X();//Enciende la señal de reloj X
          if (ax > (y - error) && ax < (y - error)) {
            /*Determina si llego
              a la posicion deseada*/
            digitalWrite(enableX, HIGH);//Enclava el motor X
            qx = false;//Enciende y apaga banderas de apoyo
            qy = true;
            break;//Romple el ciclo
          }
        }
        while (qy == true) {
          ay = sensorY();//Toma el valor del sensor Y
          motor_atras();//Enciende el motor Y hacia atras
          reloj_Y();//Enciende la señal de reloj Y
          if (ay < (z - error) && ay < (z + error)) {
            /*Determina si llego
              a la posicion deseada*/
            digitalWrite(enableY, HIGH);//Enclava el motor Y
            qy = false;//Apaga bandera de apoyo
            c = 0;//Cambia la variable del electroiman
            electroiman(c);//Apaga el electroiman
          }
        }
      }
    } else {
      if (ay < (z + lm)) {
        /*Activa el movimiento del motor
          Y hacia adelante para que llegue a la fila deceada
          mas la mitad de la casilla*/
        motor_adelante();//Enciende motor Y hacia adelante
        reloj_Y();//Enciende la señal de reloj Y
      }
      if (ay > (z - error) && ay < (z + error)) {
        /*Determian si llego
          a la posicion deseada*/
        digitalWrite(enableY, HIGH);//Enclava el motor Y
        while (true) {
          ax = sensorX();//Toma el valor del sensor X
          motor_izquierda();//Enciende el motor X hacia la izquierda
          reloj_X();//Enciende señal de reloj X
          if (ax > (y - error) && ax < (y + error)) {
            /*Determina si llego
              a la posicion deseada*/
            digitalWrite(enableX, HIGH);//Enclava motor X
            qx = false;//Enciende y apaga banderas de apoyo
            qy = true;
            break;//Rompe el ciclo
          }
        }
        while (qy == true) {
          ay = sensorY();//Toma el valor del sensor Y
          motor_atras();//Enciende el motor Y hacia atras
          reloj_Y();//Enciende la señal de reloj Y
          if (ay > (z - error) && ay < (z + error)) {
            /*Determina si llgo
              a la posicion deseada*/
            digitalWrite(enableY, HIGH);//Enclava el motor Y
            qy = false;//Apaga la bandera de apoyo
            c = 0;//Cambia la variable del electroiman
            electroiman(c);//Apaga el electroiman
          }
        }
      }
    }
  }
}
/////////FUNCION PARA REALIZAR EL MOVIMIENTO EN X SIN PIEZA/////////////////
void movimiento_X(int x) {
  qx = true;//Enciende bandera de apoyo
  casilla_X = casillaX(x);//Determina el valor de la columna
  while (qx == true) {
    ax = sensorX();//Toma el valor del sensor X
    if (ax < casilla_X)//Determina hacia donde tiene que ir
      motor_derecha();//Enciende motor X hacia la derecha
    else
      motor_izquierda();//Enciende motor X hacia la izquierda
    reloj_X();//Enciende señal de reloj X
    if (ax > (casilla_X - error) && ax < (casilla_X + error)) {
      /*Determina si llego
        a la posicion deseada*/
      digitalWrite(enableX, HIGH);//Enclava motor X
      qx = false;//Apaga bandera de apoyo
    }
  }
}
//////////FUNCION PARA REALIZAR EL MOVIMIENTO EN Y SIN PIEZA///////////////
void movimiento_Y(int x) {
  qy = true;//Enciende bandera de apoyo
  casilla_Y = casillaY(x);//Determian el valor de la fila
  while (qy == true) {
    ay = sensorY();//Toma el valor del sensor Y
    if (ay < casilla_Y)//Determina hacia donde tiene que ir
      motor_adelante();//Enciende motor Y hacia adelante
    else
      motor_atras();//Enciende motor Y hacia atras
    reloj_Y();//Enciende señal de reloj Y
    if (ay > (casilla_Y - error) && ay < (casilla_Y + error)) {
      /*Determina si llego
        a la posicion deseada*/
      digitalWrite(enableY, HIGH);//Enclava motor Y
      qy = false;//Apaga banderaa de apoyo
    }
  }
}
///////////////////////MOVIMIENTO DE CADA PIEZAS //////////////////////
void Movimiento_Pieza(int x, int y) {
  switch (x) {//Determina que pieza se va a mover
    case 0://MOVIMIENTO DEL PEON
      if (y == 1) {//Determina si la pieza capturo
        casilla_X = casillaX(pfx);//Toma el valor de la columna a llegar
        ax = sensorX();//Toma le valor del sensor X
        if (ax > casilla_X) {//Determina si se mueve a la derecha
          motor_derecha();//Activa motor X hacia la derecha
          reloj_X();//Activa señal de reloj X
          //Determina si llego al lugar deseado
          if (ax > (casilla_X - lm - error) && ax < (casilla_X - lm + error)) {
            digitalWrite(enableX, HIGH);//Enclava el motor X
            qy = true;//Activa bandera de apoyo
            while (qy == true) {
              ay = sensorY();//Toma el valor del sensor Y
              casilla_Y = casillaY(pfy);//Tomo el valor de la columna de destino
              if (ay < casilla_Y) {//Determina el sentido de movimiento
                motor_adelante();//Activa motor Y hacia adelante
                reloj_Y();//Activa la señal de reloj Y
                //Deterina si llego a la posicion deseada
                if (ay > (casilla_Y - error) && ay < (casilla_Y + error)) {
                  digitalWrite(enableY, HIGH);//Enclava el motor Y
                  qy = false;//Enciende y apaga banderas de apoyo
                  qx = true;
                }
              } else {
                motor_atras();//Enciende motor Y hacia atras
                reloj_Y();//Enciende señaol de reloj Y
                //Determina si llego a la posicion deseada
                if (ay > (casilla_Y - error) && ay < (casilla_Y + error)) {
                  digitalWrite(enableY, HIGH);//Enclava el motor Y
                  qy = false;//Enciende y apaga banderas de apoyo
                  qx = true;
                }
              }
            }
            while (qx == true) {
              motor_derecha();//Enciende motor X hacia la derecha
              reloj_X();//Enciende señal de reloj X
              //Determina si llego a la posicion deseada
              if (ax > (casilla_X - error) && ax < (casilla_X + error)) {
                digitalWrite(enableX, HIGH);//Enclava el motor X
                qx = false;//Apaga la bandera de apoyo
              }
            }
          }
        } else {//Por lo contrario a la izquierda
          motor_izquierda();//Enciende motor X hacia la izquierda
          reloj_X();//Enciende señald e reloj X
          //Determina si llego a la posicion deseada
          if (ax > (casilla_X + lm - error) && ax < (casilla_X + lm + error)) {
            digitalWrite(enableX, HIGH);//Enclava el motor X
            qy = true;//Apaga bandera de apoyo
            while (qy == true) {
              ay = sensorY();//Toma el valor del sensor Y
              casilla_Y = casillaY(pfy);//Toma el valor de la filla destino
              if (ay < casilla_Y) {//Determina el tipo de movimiento
                motor_adelante();//Enciende motor Y hacia adelante
                reloj_Y();//Enciende señal de reloj Y
                //Determina is llego a la posicion deseada
                if (ay > (casilla_Y - error) && ay < (casilla_Y + error)) {
                  digitalWrite(enableY, HIGH);//Enclava el motor Y
                  qy = false;//Enciende y apaga banderas de apoyo
                  qx = true;
                }
              } else {
                motor_atras();//Enciende motor hacia atras
                reloj_Y();//Enciende señal de reloj Y
                //Determina si llego a la posicion deseada
                if (ay > (casilla_Y - error) && ay < (casilla_Y + error)) {
                  digitalWrite(enableY, HIGH);//Enclava motor Y
                  qy = false;//Apaga y enciende banderas de apoyo
                  qx = true;
                }
              }
            }
            while (qx == true) {
              motor_izquierda();//Enciende motor hacia la izquierda
              reloj_X();//Enciende señalo de reloj X
              //Determina si llego a la posicion deseada
              if (ax > (casilla_X - error) && ax < (casilla_X + error)) {
                digitalWrite(enableX, HIGH);//Enclava motor X
                qx = false;//Apaga bandera de apoyo
              }
            }
          }
        }
      } else if (y == 0) {//Comprueba si no hay captura
        qy = true;//Enciende bandera de apoyo
        while (qy == true) {
          casilla_Y = casillaY(pfy);//Toma le valor de la fila destino
          ay = sensorY();//Toma el valor del sensor Y
          motor_adelante();//Enciende el motor Y hacia adelante
          reloj_Y();//Enciende señal de reloj Y
          //Determina si llego a la posicion deseada
          if (ay > (casilla_Y - error) && ay < (casilla_Y + error)) {
            digitalWrite(enableY, HIGH);//Enclava el motor Y
            qy = false;//Apaga bandera de apoyo
          }
        }
      }
      break;//Rompe el switch
    case 1://MOVIMIETNO DE LA TORRE
      casilla_X = casillaX(pfx);//Toma el valor de la columna destino
      casilla_Y = casillaY(pfy);//Toma el valor de la fila destino
      ax = sensorX();//Toma el valor del sensor X
      ay = sensorY();//Toma el valor del sensor Y
      if (pfy == piy) {//Determina si se tiene que mover verticalmente
        if (ay < casilla_Y) {//Determina el sentido de movimeinto vertical
          motor_adelante();//Enciende motor Y hacia adelante
          reloj_Y();//Enciende señal de reloj Y
          //Determina si llego a la posicion deseada
          if (ay > (casilla_Y - error) && ay < (casilla_Y + error))
            digitalWrite(enableY, HIGH);//Enclava el motor Y
        } else {
          motor_atras();//Enciende motor Y hacia atras
          reloj_Y();//Enciende señal de reloj Y
          //Determina si llego a la posicion deseada
          if (ay > (casilla_Y - error) && ay < (casilla_Y + error))
            digitalWrite(enableY, HIGH);//Enclava el motor Y
        }
      } else if (pfx == pfx) {//Comprueba si el movimeinto es horizontal
        if (ax < casilla_X) {//Determina el sentido de movimiento horizontal
          motor_izquierda();//Enciende motor X hacia la izquierda
          reloj_X();//Enciende señal de reloj X
          //Determina si llego a la posicion deseada
          if (ax > (casilla_X - error) && ax < (casilla_X + error))
            digitalWrite(enableX, HIGH);//Enclava el motor X
        } else {
          motor_derecha();//Enciende el motor X hacia la derecha
          reloj_X();//Enciende señal de reloj X
          //Determian si llego a la posicion deseada
          if (ax > (casilla_X - error) && ax < (casilla_X + error))
            digitalWrite(enableX, HIGH);//Enclava el motor X
        }
      }
      break;//Rompe el switch
    case 2://MOVIMIENTO DEL CABALLO
      int variable;
      casilla_X = casillaX(pfx);
      casilla_Y = casillaY(pfy);
      variable = pfy - piy;
      variable = abs(variable);
      ax = sensorX();
      if (variable == 1) {
        casilla_X = casillaX(pfx);
        casilla_Y = casillaY(pfx);
        ay = sensorY();
        if (ay < casilla_Y) {
          motor_atras();
          reloj_Y();
          if (ay > (casilla_Y - lm - error) && ay < (casilla_Y - lm + error)) {
            digitalWrite(enableY, HIGH);
            qx = true;
            while (qx == true) {
              ax = sensorX();
              if (ax > casilla_X) {
                motor_derecha();
                reloj_X();
                if (ax > (casilla_X - error) && ax < (casilla_X + error)) {
                  digitalWrite(enableX, HIGH);
                  qx = false;
                  qy = true;
                  while (qy == true) {
                    motor_atras();
                    reloj_Y();
                    ay = sensorY();
                    if (ay < (casilla_X + error) && ay > (casilla_X - error)) {
                      digitalWrite(enableY, HIGH);
                      qy = false;
                    }
                  }
                }
              } else {
                motor_izquierda();
                reloj_X();
                if (ax > (casilla_X - error) && ax < (casilla_X + error)) {
                  digitalWrite(enableX, HIGH);
                  qx = false;
                  qy = true;
                  while (qy == true) {
                    motor_atras();
                    reloj_Y();
                    ay = sensorY();
                    if (ay < (casilla_X + error) && ay > (casilla_X - error)) {
                      digitalWrite(enableY, HIGH);
                      qy = false;
                    }
                  }
                }
              }
            }
          }
        } else {
          motor_adelante();
          reloj_Y();
          if (ay > (casilla_Y + lm - error) && ay < (casilla_Y + lm + error)) {
            digitalWrite(enableY, HIGH);
            qx = true;
            while (qx == true) {
              ax = sensorX();
              if (ax > casilla_X) {
                motor_derecha();
                reloj_X();
                if (ax > (casilla_X - error) && ax < (casilla_X + error)) {
                  digitalWrite(enableX, HIGH);
                  qx = false;
                  qy = true;
                  while (qy == true) {
                    motor_adelante();
                    reloj_Y();
                    ay = sensorY();
                    if (ay < (casilla_X + error) && ay > (casilla_X - error)) {
                      digitalWrite(enableY, HIGH);
                      qy = false;
                    }
                  }
                }
              } else {
                motor_izquierda();
                reloj_X();
                if (ax > (casilla_X - error) && ax < (casilla_X + error)) {
                  digitalWrite(enableX, HIGH);
                  qx = false;
                  qy = true;
                  while (qy == true) {
                    motor_atras();
                    reloj_Y();
                    ay = sensorY();
                    if (ay < (casilla_X + error) && ay > (casilla_X - error)) {
                      digitalWrite(enableY, HIGH);
                      qy = false;
                    }
                  }
                }
              }
            }
          }
        }
      } else {
        if (ax > casilla_X) {
          motor_derecha();
          reloj_X();
          if (ax > (casilla_X + lm - error) && ax < (casilla_X + lm + error)) {
            digitalWrite(enableX, HIGH);
            qy = true;
            while (qy == true) {
              ay = sensorY();
              if (ay < casilla_Y) {
                motor_atras();
                reloj_Y();
                if (ay < (casilla_Y - lm + error) && ay > (casilla_Y - lm - error)) {
                  digitalWrite(enableY, HIGH);
                  qy = false;
                  qx = true;
                  while (qx == true) {
                    motor_derecha();
                    reloj_X();
                    ax = sensorX();
                    if (ax < (casilla_X + error) && ax > (casilla_X - error)) {
                      digitalWrite(enableY, HIGH);
                      qx = false;
                    }
                  }
                }
              } else {
                motor_adelante();
                reloj_Y();
                if (ay > (casilla_Y - error) && ay < (casilla_Y + error)) {
                  digitalWrite(enableY, HIGH);
                  qy = false;
                  qx = true;
                  while (qx == true) {
                    motor_derecha();
                    reloj_X();
                    ax = sensorX();
                    if (ax < (casilla_X + error) && ax > (casilla_X - error)) {
                      digitalWrite(enableY, HIGH);
                      qx = false;
                    }
                  }
                }
              }
            }
          }
        } else {
          motor_izquierda();
          reloj_X();
          if (ax > (casilla_X - lm - error) && ax < (casilla_X - lm + error)) {
            digitalWrite(enableX, HIGH);
            qy = true;
            while (qy == true) {
              ay = sensorY();
              if (ay > casilla_Y) {
                motor_atras();
                reloj_Y();
                if (ay > (casilla_Y - error) && ay < (casilla_Y + error)) {
                  digitalWrite(enableY, HIGH);
                  qy = false;
                  qx = true;
                  while (qx == true) {
                    motor_izquierda();
                    reloj_X();
                    ax = sensorX();
                    if (ax < (casilla_X - lm + error) && ax > (casilla_X - lm - error)) {
                      digitalWrite(enableY, HIGH);
                      qx = false;
                    }
                  }
                }
              } else {
                motor_adelante();
                reloj_Y();
                if (ay > (casilla_Y - error) && ay < (casilla_Y + error)) {
                  digitalWrite(enableY, HIGH);
                  qy = false;
                  qx = true;
                  while (qx == true) {
                    motor_izquierda();
                    reloj_X();
                    ax = sensorX();
                    if (ax < (casilla_X + error) && ax > (casilla_X - error)) {
                      digitalWrite(enableY, HIGH);
                      qx = false;
                    }
                  }
                }
              }
            }
          }
        }
      }
      break;
    case 3: //Movimietno del alfil
      long variable1;
      casilla_X = casillaX(pfx);
      casilla_Y = casillaY(pfy);
      variable1 = casillaX(pix);
      ax = sensorX();
      if (ax > casilla_X) {
        motor_derecha();
        reloj_X();
        if (ax > (variable1 - lm - error) && ax < (variable1 - lm + error)) {
          digitalWrite(enableX, HIGH);
          qy = true;
          while (qy == true) {
            ay = sensorY();
            if (ay > casilla_Y) {
              motor_adelante();
              reloj_Y();
              if (ay > (casilla_Y + lm - error) && ay < (casilla_Y + lm + error)) {
                digitalWrite(enableY, HIGH);
                qy = false;
                qx = true;
                while (qx == true) {
                  motor_derecha();
                  reloj_X();
                  ax = sensorX();
                  if (ax < (casilla_X + error) && ax > (casilla_X - error)) {
                    digitalWrite(enableX, HIGH);
                    qx = false;
                    qy = true;
                    while (qy == true) {
                      motor_adelante();
                      reloj_Y();
                      ay = sensorY();
                      if (ay > (casilla_Y - error) && ay < (casilla_Y + error)) {
                        digitalWrite(enableY, HIGH);
                        qy = false;
                      }
                    }
                  }
                }
              }
            } else {
              motor_atras();
              reloj_Y();
              if (ay > (casilla_Y - lm - error) && ay < (casilla_Y - lm + error)) {
                digitalWrite(enableY, HIGH);
                qy = false;
                qx = true;
                while (qx == true) {
                  motor_derecha();
                  reloj_X();
                  ax = sensorX();
                  if (ax < (casilla_X + error) && ax > (casilla_X - error)) {
                    digitalWrite(enableX, HIGH);
                    qx = false;
                    qy = true;
                    while (qy == true) {
                      motor_atras();
                      reloj_Y();
                      ay = sensorY();
                      if (ay > (casilla_Y - error) && ay < (casilla_Y + error)) {
                        digitalWrite(enableY, HIGH);
                        qy = false;
                      }
                    }
                  }
                }
              }
            }
          }
        }
      } else {
        motor_izquierda();
        reloj_X();
        if (ax > (variable1 + lm - error) && ax < (variable1 + lm + error)) {
          digitalWrite(enableX, HIGH);
          qy = true;
          while (qy == true) {
            ay = sensorY();
            if (ay > casilla_Y) {
              motor_adelante();
              reloj_Y();
              if (ay > (casilla_Y + lm - error) && ay < (casilla_Y + lm + error)) {
                digitalWrite(enableY, HIGH);
                qy = false;
                qx = true;
                while (qx == true) {
                  motor_izquierda();
                  reloj_X();
                  ax = sensorX();
                  if (ax < (casilla_X + error) && ax > (casilla_X - error)) {
                    digitalWrite(enableX, HIGH);
                    qx = false;
                    qy = true;
                    while (qy == true) {
                      motor_adelante();
                      reloj_Y();
                      ay = sensorY();
                      if (ay > (casilla_Y - error) && ay < (casilla_Y + error)) {
                        digitalWrite(enableY, HIGH);
                        qy = false;
                      }
                    }
                  }
                }
              }
            } else {
              motor_atras();
              reloj_Y();
              if (ay > (casilla_Y - lm - error) && ay < (casilla_Y - lm + error)) {
                digitalWrite(enableY, HIGH);
                qy = false;
                qx = true;
                while (qx == true) {
                  motor_izquierda();
                  reloj_X();
                  ax = sensorX();
                  if (ax < (casilla_X + error) && ax > (casilla_X - error)) {
                    digitalWrite(enableX, HIGH);
                    qx = false;
                    qy = true;
                    while (qy == true) {
                      motor_atras();
                      reloj_Y();
                      ay = sensorY();
                      if (ay > (casilla_Y - error) && ay < (casilla_Y + error)) {
                        digitalWrite(enableY, HIGH);
                        qy = false;
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
      break;
    case 4://MOVIMIENTO DE LA DAMA Y REY
      if (pix == pfx || piy == pfy) {
        casilla_X = casillaX(pfx);
        casilla_Y = casillaY(pfy);
        ax = sensorX();
        ay = sensorY();
        if (pfy == piy) {
          if (ay < casilla_Y) {
            motor_adelante();
            reloj_Y();
            if (ay > (casilla_Y - error) && ay < (casilla_Y + error))
              digitalWrite(enableY, HIGH);
          } else {
            motor_atras();
            reloj_Y();
            if (ay > (casilla_Y - error) && ay < (casilla_Y + error))
              digitalWrite(enableY, HIGH);
          }
        } else if (pfx == pfx) {
          if (ax < casilla_X) {
            motor_izquierda();
            reloj_X();
            if (ax > (casilla_X - error) && ax < (casilla_X + error))
              digitalWrite(enableX, HIGH);
          } else {
            motor_derecha();
            reloj_X();
            if (ax > (casilla_X - error) && ax < (casilla_X + error))
              digitalWrite(enableX, HIGH);
          }
        }
      } else {
        long variable1;
        casilla_X = casillaX(pfx);
        casilla_Y = casillaY(pfy);
        variable1 = casillaX(pix);
        ax = sensorX();
        if (ax > casilla_X) {
          motor_derecha();
          reloj_X();
          if (ax > (variable1 - lm - error) && ax < (variable1 - lm + error)) {
            digitalWrite(enableX, HIGH);
            qy = true;
            while (qy == true) {
              ay = sensorY();
              if (ay > casilla_Y) {
                motor_adelante();
                reloj_Y();
                if (ay > (casilla_Y + lm - error) && ay < (casilla_Y + lm + error)) {
                  digitalWrite(enableY, HIGH);
                  qy = false;
                  qx = true;
                  while (qx == true) {
                    motor_derecha();
                    reloj_X();
                    ax = sensorX();
                    if (ax < (casilla_X + error) && ax > (casilla_X - error)) {
                      digitalWrite(enableX, HIGH);
                      qx = false;
                      qy = true;
                      while (qy == true) {
                        motor_adelante();
                        reloj_Y();
                        ay = sensorY();
                        if (ay > (casilla_Y - error) && ay < (casilla_Y + error)) {
                          digitalWrite(enableY, HIGH);
                          qy = false;
                        }
                      }
                    }
                  }
                }
              } else {
                motor_atras();
                reloj_Y();
                if (ay > (casilla_Y - lm - error) && ay < (casilla_Y - lm + error)) {
                  digitalWrite(enableY, HIGH);
                  qy = false;
                  qx = true;
                  while (qx == true) {
                    motor_derecha();
                    reloj_X();
                    ax = sensorX();
                    if (ax < (casilla_X + error) && ax > (casilla_X - error)) {
                      digitalWrite(enableX, HIGH);
                      qx = false;
                      qy = true;
                      while (qy == true) {
                        motor_atras();
                        reloj_Y();
                        ay = sensorY();
                        if (ay > (casilla_Y - error) && ay < (casilla_Y + error)) {
                          digitalWrite(enableY, HIGH);
                          qy = false;
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        } else {
          motor_izquierda();
          reloj_X();
          if (ax > (variable1 + lm - error) && ax < (variable1 + lm + error)) {
            digitalWrite(enableX, HIGH);
            qy = true;
            while (qy == true) {
              ay = sensorY();
              if (ay > casilla_Y) {
                motor_adelante();
                reloj_Y();
                if (ay > (casilla_Y + lm - error) && ay < (casilla_Y + lm + error)) {
                  digitalWrite(enableY, HIGH);
                  qy = false;
                  qx = true;
                  while (qx == true) {
                    motor_izquierda();
                    reloj_X();
                    ax = sensorX();
                    if (ax < (casilla_X + error) && ax > (casilla_X - error)) {
                      digitalWrite(enableX, HIGH);
                      qx = false;
                      qy = true;
                      while (qy == true) {
                        motor_adelante();
                        reloj_Y();
                        ay = sensorY();
                        if (ay > (casilla_Y - error) && ay < (casilla_Y + error)) {
                          digitalWrite(enableY, HIGH);
                          qy = false;
                        }
                      }
                    }
                  }
                }
              } else {
                motor_atras();
                reloj_Y();
                if (ay > (casilla_Y - lm - error) && ay < (casilla_Y - lm + error)) {
                  digitalWrite(enableY, HIGH);
                  qy = false;
                  qx = true;
                  while (qx == true) {
                    motor_izquierda();
                    reloj_X();
                    ax = sensorX();
                    if (ax < (casilla_X + error) && ax > (casilla_X - error)) {
                      digitalWrite(enableX, HIGH);
                      qx = false;
                      qy = true;
                      while (qy == true) {
                        motor_atras();
                        reloj_Y();
                        ay = sensorY();
                        if (ay > (casilla_Y - error) && ay < (casilla_Y + error)) {
                          digitalWrite(enableY, HIGH);
                          qy = false;
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
      break;
  }
}
