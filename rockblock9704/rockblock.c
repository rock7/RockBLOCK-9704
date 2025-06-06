#include <Python.h>
#include "rockblock_9704.h"

PyAPI_FUNC(int)
_PyArg_ParseTuple_SizeT(PyObject *args, const char *format, ...);

PyAPI_FUNC(PyObject *)
_Py_BuildValue_SizeT(const char *, ...);

static PyObject *py_getSignal(PyObject *self, PyObject *args) {

    int result = rbGetSignal();

    return Py_BuildValue("i", result);

}

static PyObject *py_rbBegin(PyObject *self, PyObject *args) {

    int result;
    char * port;

    if (!_PyArg_ParseTuple_SizeT(args, "s", &port)) {

        return NULL;

    }

    result = rbBegin(port);

    return Py_BuildValue("i", result);

}

static PyObject *py_rbEnd(PyObject *self, PyObject *args) {

    int result;

    result = rbEnd();

    return Py_BuildValue("i", result);

}
#ifdef RB_GPIO
static PyObject *py_rbBeginGpio(PyObject *self, PyObject *args) {

    int result;
    char * port;
    PyObject *gpioDict;
    int timeout;

    if (!_PyArg_ParseTuple_SizeT(args, "sOi", &port, &gpioDict, &timeout)) {

        return NULL;

    }

    if (!PyDict_Check(gpioDict)) {

        PyErr_SetString(PyExc_TypeError, "Expected a dictionary for gpioInfo");
        return NULL;

    }

    rbGpioTable_t gpioInfo;
    const char *chip;
    int pin;
    PyObject *tuple;

    tuple = PyDict_GetItemString(gpioDict, "powerEnable");
    if (!_PyArg_ParseTuple_SizeT(tuple, "si", &chip, &pin)){

        return NULL;

    }
    strncpy(gpioInfo.powerEnable.chip, chip, GPIO_CHIP_MAX_LEN);
    gpioInfo.powerEnable.pin = (uint8_t)pin;

    tuple = PyDict_GetItemString(gpioDict, "iridiumEnable");
    if (!_PyArg_ParseTuple_SizeT(tuple, "si", &chip, &pin)){

        return NULL;

    }
    strncpy(gpioInfo.iridiumEnable.chip, chip, GPIO_CHIP_MAX_LEN);
    gpioInfo.iridiumEnable.pin = (uint8_t)pin;

    tuple = PyDict_GetItemString(gpioDict, "booted");
    if (!_PyArg_ParseTuple_SizeT(tuple, "si", &chip, &pin)) {

        return NULL;

    }
    strncpy(gpioInfo.booted.chip, chip, GPIO_CHIP_MAX_LEN);
    gpioInfo.booted.pin = (uint8_t)pin;

    result = rbBeginGpio(port, &gpioInfo, timeout);

    return Py_BuildValue("i", result);

}

static PyObject *py_rbEndGpio(PyObject *self, PyObject *args) {


    int result;
    PyObject *gpioDict;

    if (!_PyArg_ParseTuple_SizeT(args, "O", &gpioDict)) {

        return NULL;

    }

    if (!PyDict_Check(gpioDict))
    {
        PyErr_SetString(PyExc_TypeError, "Expected a dictionary for gpioInfo");
        return NULL;

    }

    rbGpioTable_t gpioInfo;
    const char *chip;
    int pin;
    PyObject *tuple;

    tuple = PyDict_GetItemString(gpioDict, "powerEnable");
    if (!_PyArg_ParseTuple_SizeT(tuple, "si", &chip, &pin)){

        return NULL;

    }
    strncpy(gpioInfo.powerEnable.chip, chip, GPIO_CHIP_MAX_LEN);
    gpioInfo.powerEnable.pin = (uint8_t)pin;

    tuple = PyDict_GetItemString(gpioDict, "iridiumEnable");
    if (!_PyArg_ParseTuple_SizeT(tuple, "si", &chip, &pin)){

        return NULL;

    }
    strncpy(gpioInfo.iridiumEnable.chip, chip, GPIO_CHIP_MAX_LEN);
    gpioInfo.iridiumEnable.pin = (uint8_t)pin;

    tuple = PyDict_GetItemString(gpioDict, "booted");
    if (!_PyArg_ParseTuple_SizeT(tuple, "si", &chip, &pin)) {

        return NULL;

    }
    strncpy(gpioInfo.booted.chip, chip, GPIO_CHIP_MAX_LEN);
    gpioInfo.booted.pin = (uint8_t)pin;

    result = rbEndGpio(&gpioInfo);

    return Py_BuildValue("i", result);

}
#endif
static PyObject *py_sendMessage(PyObject *self, PyObject *args) {

    int result;
    char * data;
    Py_ssize_t length;
    int timeout;

    if (!_PyArg_ParseTuple_SizeT(args, "s#i", &data, &length, &timeout)) {

        return NULL;

    }

    result = rbSendMessage(data, length, timeout);

    return Py_BuildValue("i", result);

}

static PyObject *py_sendMessageAny(PyObject *self, PyObject *args) {

    int result, topic;
    char * data;
    Py_ssize_t length;
    int timeout;

    if (!_PyArg_ParseTuple_SizeT(args, "is#i", &topic, &data, &length, &timeout)) {

        return NULL;

    }

    result = rbSendMessageAny(topic, data, length, timeout);

    return Py_BuildValue("i", result);

}

static PyObject *py_receiveMessage(PyObject *self, PyObject *args) {

    char* mtBuffer;

    const size_t mtLength = rbReceiveMessage(&mtBuffer);

    if ((mtLength > 0) && (mtBuffer != NULL)) {

        PyObject* res = _Py_BuildValue_SizeT("y#", mtBuffer, mtLength);

        return res;

    }

    return Py_BuildValue("y", NULL);

}

static PyObject *py_receiveMessageWithTopic(PyObject *self, PyObject *args) {

    int topic;

    if (!_PyArg_ParseTuple_SizeT(args, "i", &topic)) {

        return NULL;

    }

    char* mtBuffer;

    const size_t mtLength = rbReceiveMessageWithTopic(&mtBuffer, topic);

    if ((mtLength > 0) && (mtBuffer != NULL)) {

        PyObject* res = _Py_BuildValue_SizeT("y#", mtBuffer, mtLength);

        return res;

    }

    return Py_BuildValue("y", NULL);

}

static PyObject *py_getHardwareVersion(PyObject *self, PyObject *args) {

    char * result = rbGetHwVersion();
    return Py_BuildValue("s", result);

}

static PyObject *py_getSerialNumber(PyObject *self, PyObject *args) {

    char * result = rbGetSerialNumber();
    return Py_BuildValue("s", result);

}

static PyObject *py_getImei(PyObject *self, PyObject *args) {

    char * result = rbGetImei();
    return Py_BuildValue("s", result);

}

static PyObject *py_getBoardTemp(PyObject *self, PyObject *args) {

    int result = rbGetBoardTemp();
    return Py_BuildValue("i", result);

}

static PyObject *py_getCardPresent(PyObject *self, PyObject *args) {

  int result = rbGetCardPresent();
  return Py_BuildValue("i", result);

}

static PyObject *py_getSimConnected(PyObject *self, PyObject *args) {

  int result = rbGetSimConnected();
  return Py_BuildValue("i", result);

}

static PyObject *py_getIccid(PyObject *self, PyObject *args) {

  char * result = rbGetIccid();
  return Py_BuildValue("s", result);

}

static PyObject *py_getFirmwareVersion(PyObject *self, PyObject *args) {

  char * result = rbGetFirmwareVersion();
  return Py_BuildValue("s", result);

}

static PyObject *py_resyncServiceConfig(PyObject *self, PyObject *args) {
  int result = rbResyncServiceConfig();
  return Py_BuildValue("i", result);
}

static PyMethodDef rockblockMethods[] = {
    {"get_signal", py_getSignal, METH_VARARGS, "Function for getting the modem signal quality"},
    {"begin", py_rbBegin, METH_VARARGS, "Function for starting serial connection to modem"},
    {"end", py_rbEnd, METH_VARARGS, "Function for ending serial connection to modem"},
#ifdef RB_GPIO
    {"begin_gpio", py_rbBeginGpio, METH_VARARGS, "Function for starting serial connection to modem and initaliasing PiHat manually"},
    {"end_gpio", py_rbEndGpio, METH_VARARGS, "Function for ending serial connection to modem and deinitaliasing PiHat manually"},
#endif
    {"send_message", py_sendMessage, METH_VARARGS, "Function for sending mo data to the default topic"},
    {"send_message_any", py_sendMessageAny, METH_VARARGS, "Function for sending mo data to any topic"},
    {"receive_message", py_receiveMessage, METH_VARARGS, "Function for receiving mt data"},
    {"receive_message_with_topic", py_receiveMessageWithTopic, METH_VARARGS, "Function for receiving mt data from topic"},
    {"get_hardware_version", py_getHardwareVersion, METH_VARARGS, "Function for getting hardware version"},
    {"get_serial_number", py_getSerialNumber, METH_VARARGS, "Function for getting serial number"},
    {"get_imei", py_getImei, METH_VARARGS, "Function for getting IMEI"},
    {"get_board_temp", py_getBoardTemp, METH_VARARGS, "Function for getting board temp"},
    {"get_card_present", py_getCardPresent, METH_VARARGS, "Function for checking if card present"},
    {"get_sim_connected", py_getSimConnected, METH_VARARGS, "Function for getting sim connected"},
    {"get_iccid", py_getIccid, METH_VARARGS, "Function for getting iccid"},
    {"get_firmware_version", py_getFirmwareVersion, METH_VARARGS, "Function for getting firmware version"},
    {"resync_service_config", py_resyncServiceConfig, METH_VARARGS, "Function for forcing service configuration resync"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef rockblock = {
    PyModuleDef_HEAD_INIT,
    "rockblock",
    "Custom rockblock module",
    -1,
    rockblockMethods
};

PyMODINIT_FUNC PyInit_rockblock(void)
{
    return PyModule_Create(&rockblock);
}
