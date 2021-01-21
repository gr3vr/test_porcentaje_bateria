#define vBatPin 34    // Pin ADC lectura voltaje
#define vBatMax 2418  // Valor máximo leído del ADC cuando la batería está cargada completamente (4.2v)
#define vBatMin 1880  // Valor mínimo leído del ADC cuando la batería está descargada completamente (3.3v)

#include <SimpleKalmanFilter.h>

/*
  Librería disponible en: https://github.com/denyssene/SimpleKalmanFilter
  SimpleKalmanFilter kf = SimpleKalmanFilter(e_mea, e_est, q);

  e_mea: Measurement Uncertainty - How much do we expect to our measurement vary

  e_est:  Estimation Uncertainty - Can be initilized with the same value as e_mea
        since the kalman filter will adjust its value.

  q:  Process Variance - usually a small number between 0.001 and 1
    how fast your measurement moves.
    Recommended 0.01. Should be tunned to your needs.
*/

SimpleKalmanFilter kf = SimpleKalmanFilter(1, 1, 0.01);

void setup() {
  Serial.begin(9600);
  delay(10);
}

int valBat() {
  float val, aux, i, valMin;
  for (i = 1; i < 500; i++) {             // Realiza 500 lecturas para suavizar el valor
    aux = analogRead(vBatPin);
    val += kf.updateEstimate(aux);        // Y va sumando cada lectura con el primer filtro
    delayMicroseconds(200);
  }
  val /= (i - 1.0);                       // Divide la suma de la lectura entre la cant de iteraciones
  val = kf.updateEstimate(val);           // Luego se filtra una última vez
  
  if (val > vBatMax) {                    // Condicionales para no pasar los valores máx y min
    val = vBatMax;
  }
  if (val < vBatMin) {
    val = vBatMin;
  }
  /*
    Serial.print(val);
    Serial.print(" ");
  */
  return map(val, vBatMin, vBatMax, 0, 100);  // Devuelve el porcentaje de 0 - 100 del valor medido
}

//  Función para obtener los valores de los límites máximos y mínimos del adc
//  cuando se lee la batería, realiza un promedio de 1000 iteraciones.

int testV() {
  int a, v, i;
  for (i = 1; i < 1000; i++) {
    a = analogRead(vBatPin);
    v += kf.updateEstimate(a);
  }
  return v /= (i - 1);
}

void loop() {
  Serial.print(valBat());           // Comentar cuando no se tenga los valores vBatMax y vBatMin
  Serial.println(" %");
  //Serial.println(testV());        // Comentar cuando ya se tenga los valores Max y Min
  delay(2000);
}
