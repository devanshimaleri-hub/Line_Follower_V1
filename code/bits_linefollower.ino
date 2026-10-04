const byte leftmotor1 = 8;
const byte leftmotor2 = 9;
const byte rightmotor1 = 12;
const byte rightmotor2 = 13;
const byte leftspeedpin = 5;
const byte rightspeedpin = 6;

int base_speed = 150;

int error = 0;
int previous_error = 0;

float kp =40;
float kd =50;

const byte ir[7] = {7,17,4,3,2,16,15};

bool inverted = false;
int invertedCounter = 0; 

void setup() {
  // put your setup code here, to run once:

  pinMode(leftmotor1,OUTPUT);
  pinMode(leftmotor2,OUTPUT);
  pinMode(rightmotor1,OUTPUT);
  pinMode(rightmotor2,OUTPUT);
  pinMode(leftspeedpin,OUTPUT);
  pinMode(rightspeedpin,OUTPUT);

  for (int i = 0; i < 7; i++) {
  pinMode(ir[i], INPUT);

}
}

void loop() {
  // put your main code here, to run repeatedly:
  follow_line();

}


void moveforward(int rightspeed, int leftspeed)
{
  digitalWrite(leftmotor1,LOW);
  digitalWrite(leftmotor2,HIGH);
  digitalWrite(rightmotor1,LOW);
  digitalWrite(rightmotor2,HIGH);
  analogWrite(rightspeedpin,rightspeed);
  analogWrite(leftspeedpin,leftspeed);
}

float find_error()
{
 int weights[7] = {-8,-6,-2,0,2,6,8};
  float sum = 0;
  float count = 0;

  int centre = digitalRead(ir[3]);

  // ── Detect if on circle/thick line ──────────────────────
  bool onCircle = ((digitalRead(ir[2]) == digitalRead(ir[4])) &&  (digitalRead(ir[3]) != digitalRead(ir[4])));
                

  // If on circle, ignore outermost sensors (index 0 and 6)
  int startIdx = onCircle ? 1 : 0;
  int endIdx   = onCircle ? 5 : 6;   // inclusive
  // ─────────────────────────────────────────────────────────

  // ── Inverted zone detection via outer sensors ──────────────
  if (digitalRead(ir[0]) == LOW && digitalRead(ir[6]) == LOW) {
    invertedCounter++;
  } else {
    invertedCounter = 0;   // reset the moment outer sensors change
  }

  if (invertedCounter > 3) {
    inverted = true;
  } else {
    inverted = false;
  }
  // ───────────────────────────────────────────────────────────


  for (int i = startIdx; i <= endIdx; i++)
  {
    if (digitalRead(ir[i]) != centre)
    {
      sum += weights[i];
      count++;
    }
  }

  if (count == 0)
  {
    if (previous_error > 0) return 8;
    else return -8;
  }

  return sum/count;

}

void follow_line()
{
  error = find_error();
  int derivative = error - previous_error;

  int correction = kp*error + kd*derivative;
  correction = constrain(correction,-150,150);

  int rightmotorspeed = base_speed + correction;
  int leftmotorspeed = base_speed - correction;

  rightmotorspeed = constrain(rightmotorspeed,0,255);
  leftmotorspeed = constrain(leftmotorspeed,0,255);

  moveforward(rightmotorspeed,leftmotorspeed);

  previous_error = error;
}