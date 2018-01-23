/*

  This program expects pulses of logic high coming from a bill acceptor into a GPIO pin on an arduino leonardo or anything with an Atema32u4 and an arduino compatible boot loader.
  It counts the number of pulses, pretends to be a usb keyboard and types the dollar amount followed by a newline after each bill has been accepted e.g: "5.00\n"
*/

// The pin on the arduino where CREDIT (-) [Common] is connected
#define BILL_ACCEPTOR_PIN (15)
#define COIN_ACCEPTOR_PIN (2)

int cents_received = 0; // Counts how many cents have been received

void setup() {
  pinMode(BILL_ACCEPTOR_PIN, INPUT);
  Serial.begin(115200); // You can comment all the Keyboard lines and uncomment all the serial lines to make it print to serial instead (useful for debugging)
}

void loop() {
  cents_received = bill_acceptor();
  if (cents_received > 0) {
    Serial.print(cents_received);
    Serial.println(" bill_acceptor");
  }
  cents_received = coin_acceptor();
  if (cents_received > 0) {
    Serial.print(cents_received);
    Serial.println(" coin_acceptor");
  }
}

int bill_acceptor() {
  static int cents_per_pulse = 100; // how many cents per pulse. for most bill acceptors this is 100 or $1 per pulse, but it can often be configured and coin acceptors will be different
  static int min_pulse_width = 40; // the minimum pulse width to acccept
  static int max_pulse_width = 60; // the maximum pulse width to accept
  static int debounce_speed = 4; // ignore changes in input line state that happen faster than this
  static int pulse_count = 0; // how many pulses have been received so far in this pulse train
  static unsigned long pulse_duration; // how long was the last pulse
  static unsigned long pulse_begin = 0; // when did the last pulse begin
  static unsigned long pulse_end = 0; // if they pulse was within min and max pulse width, when did it end
  static unsigned long curtime; // what is the current time
  static int post_pulse_pause = 300; // how long to wait after last pulse before sending pulse count
  static int pulse_state; // what is the current input line state (1 for high, 0 for low)
  static int last_state = 0; // what was the last input line state
  static int whole_dollars; // how many whole dollars were received
  static int remaining_cents; // how many remaining cents were received
  static char out_str[8];
  static int bill_cents_received = 0;

  pulse_state = digitalRead(BILL_ACCEPTOR_PIN);
  curtime = millis();
  if((pulse_state == 1) && (last_state == 0)) { // this means we entered a new pulse
    pulse_begin = curtime; // save the begin time of the pulse
    last_state = 1; // set the previous state
  } else if((pulse_state == 0) && (last_state == 1)) { // this means a pulse just ended
    pulse_duration = curtime - pulse_begin; // calculate pulse duration
    if(pulse_duration > debounce_speed) { // ensure that we don't change state for very short (false) pulses (this is called debouncing)
      last_state = 0;
    }
    if((pulse_duration > min_pulse_width) && (pulse_duration < max_pulse_width)) { // check if the pulse width is between the minimum and maximum
      pulse_end = curtime; // save the end time of the pulse
      pulse_count++; // increment the pulse counter
    }
  }

  if((pulse_end > 0) && (curtime - pulse_end > post_pulse_pause)) { // check if we've waited long enough that we don't expect any further pulses to be forthcoming

    bill_cents_received += pulse_count * cents_per_pulse; // count the cents

    int return_bill_cents_received = cents_received;
    bill_cents_received = 0; // reset cents_received so it's ready for next payment
    pulse_end = 0;
    pulse_count = 0;

    return return_bill_cents_received;
  } else {
    return 0;
  }
}

int coin_acceptor() {
  static int cents_per_pulse = 100; // how many cents per pulse. for most bill acceptors this is 100 or $1 per pulse, but it can often be configured and coin acceptors will be different
  static int min_pulse_width = 40; // the minimum pulse width to acccept
  static int max_pulse_width = 60; // the maximum pulse width to accept
  static int debounce_speed = 4; // ignore changes in input line state that happen faster than this
  static int pulse_count = 0; // how many pulses have been received so far in this pulse train
  static unsigned long pulse_duration; // how long was the last pulse
  static unsigned long pulse_begin = 0; // when did the last pulse begin
  static unsigned long pulse_end = 0; // if they pulse was within min and max pulse width, when did it end
  static unsigned long curtime; // what is the current time
  static int post_pulse_pause = 300; // how long to wait after last pulse before sending pulse count
  static int pulse_state; // what is the current input line state (1 for high, 0 for low)
  static int last_state = 0; // what was the last input line state
  static int whole_dollars; // how many whole dollars were received
  static int remaining_cents; // how many remaining cents were received
  static char out_str[8];
  static int coin_cents_received = 0;

  pulse_state = digitalRead(COIN_ACCEPTOR_PIN);
  curtime = millis();
  if((pulse_state == 1) && (last_state == 0)) { // this means we entered a new pulse
    pulse_begin = curtime; // save the begin time of the pulse
    last_state = 1; // set the previous state
  } else if((pulse_state == 0) && (last_state == 1)) { // this means a pulse just ended
    pulse_duration = curtime - pulse_begin; // calculate pulse duration
    if(pulse_duration > debounce_speed) { // ensure that we don't change state for very short (false) pulses (this is called debouncing)
      last_state = 0;
    }
    if((pulse_duration > min_pulse_width) && (pulse_duration < max_pulse_width)) { // check if the pulse width is between the minimum and maximum
      pulse_end = curtime; // save the end time of the pulse
      pulse_count++; // increment the pulse counter
    }
  }

  if((pulse_end > 0) && (curtime - pulse_end > post_pulse_pause)) { // check if we've waited long enough that we don't expect any further pulses to be forthcoming

    coin_cents_received += pulse_count * cents_per_pulse; // count the cents

    int return_coin_cents_received = cents_received;
    coin_cents_received = 0; // reset cents_received so it's ready for next payment
    pulse_end = 0;
    pulse_count = 0;

    return return_coin_cents_received;
  } else {
    return 0;
  }
}
