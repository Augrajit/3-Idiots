#ifndef DINING_SYSTEM_H
#define DINING_SYSTEM_H

#include <Arduino.h>
#include "../config/data_types.h"

class DiningSystem {
private:
  SystemState current_state;
  SystemConfig system_config;
  String current_rfid_uid;
  String current_face_image;
  FaceVerificationResult current_verification_result;
  FraudCheckResult current_fraud_result;
  Transaction current_transaction;
  unsigned long last_state_change;
  
  void state_idle();
  void state_waiting_for_card();
  void state_capturing_face();
  void state_verifying();
  void state_decision();
  void state_manager_approval_wait();
  void state_transaction_log();
  void transition_to(SystemState next_state);
  void handle_keyboard_input(int key);
  void create_transaction(String status, String reason);
  
public:
  DiningSystem();
  void init(SystemConfig config);
  void update();
  SystemState get_state();
};

#endif

