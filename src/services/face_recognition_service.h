#ifndef FACE_RECOGNITION_SERVICE_H
#define FACE_RECOGNITION_SERVICE_H

#include <Arduino.h>
#include "../config/data_types.h"

FaceVerificationResult verify_face_with_rfid(String rfid_uid);

#endif

