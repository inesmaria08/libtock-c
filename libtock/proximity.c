#include "proximity.h"
#include "tock.h"
#include <stdio.h>

struct thresholds {
  uint8_t lower_threshold;
  uint8_t higher_threshold;
};

// structure to store threshold values to be sent to the driver
static struct thresholds threshes = {.lower_threshold = 0, .higher_threshold = 175};

struct data {
  bool fired;
  int proximity;
};

static struct data result = {.fired = false};

// Internal callback for faking synchronous reads
static void cb(int proximity,
               __attribute__((unused)) int unused,
               __attribute__((unused)) int unused1,
               void *ud) {
  struct data *data = (struct data *)ud;
  data->proximity = proximity;
  data->fired     = true;
}

int proximity_set_callback(subscribe_cb callback, void *callback_args) {
  subscribe_return_t sub = subscribe2(DRIVER_NUM_PROXIMITY, 0, callback, callback_args);
  if (sub.success) {
    return TOCK_SUCCESS;
  } else {
    return tock_error_to_rcode(sub.error);
  }
}

int proximity_read(void) {
  syscall_return_t com = command2(DRIVER_NUM_PROXIMITY, 1, 0, 0);
  if (com.type == TOCK_SYSCALL_SUCCESS) {
    return TOCK_SUCCESS;
  } else if (com.type > TOCK_SYSCALL_SUCCESS) {
    // Returned an incorrect success code
    return TOCK_FAIL;
  } else {
    return tock_error_to_rcode(com.data[0]);
  }
}

int proximity_read_on_interrupt(void) {
  syscall_return_t com = command2(DRIVER_NUM_PROXIMITY, 2, threshes.lower_threshold, threshes.higher_threshold);
  if (com.type == TOCK_SYSCALL_SUCCESS) {
    return TOCK_SUCCESS;
  } else if (com.type > TOCK_SYSCALL_SUCCESS) {
    // Returned an incorrect success code
    return TOCK_FAIL;
  } else {
    return tock_error_to_rcode(com.data[0]);
  }
}

int proximity_set_interrupt_thresholds(uint8_t lower, uint8_t upper) {
  threshes.lower_threshold  = lower;
  threshes.higher_threshold = upper;
  return 0;
}

int proximity_read_sync(uint8_t *proximity) {
  int err;
  result.fired = false;

  err = proximity_set_callback(cb, (void *)&result);
  if (err < 0) {
    return err;
  }

  err = proximity_read();
  if (err < 0) {
    return err;
  }

  yield_for(&result.fired);

  *proximity = result.proximity;

  return 0;
}

int proximity_read_on_interrupt_sync(uint8_t *proximity) {

  int err;
  result.fired = false;

  err = proximity_set_callback(cb, (void *)&result);
  if (err < 0) {
    return err;
  }

  err = proximity_read_on_interrupt();
  if (err < 0) {
    return err;
  }

  yield_for(&result.fired);

  *proximity = result.proximity;

  return 0;
}
