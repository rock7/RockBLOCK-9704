#include <Python.h>
#include "rockblock_9704.h"

PyAPI_FUNC(int)
_PyArg_ParseTuple_SizeT(PyObject *args, const char *format, ...);

PyAPI_FUNC(PyObject *)
_Py_BuildValue_SizeT(const char *, ...);

static PyObject *py_messageProvisioning_cb = NULL;
static PyObject *py_moMessageComplete_cb = NULL;
static PyObject *py_mtMessageComplete_cb = NULL;
static PyObject *py_constellationState_cb = NULL;

//global callback structure
static rbCallbacks_t g_callbacks = {
    .messageProvisioning = NULL,
    .moMessageComplete = NULL,
    .mtMessageComplete = NULL,
    .constellationState = NULL
};

void message_provisioning_callback(const jsprMessageProvisioning_t *messageProvisioning) {

    if (py_messageProvisioning_cb && PyCallable_Check(py_messageProvisioning_cb)) {

        PyGILState_STATE gstate = PyGILState_Ensure();

        PyObject *pyProvisioningList = PyList_New(messageProvisioning->topicCount);

        for (int i = 0; i < messageProvisioning->topicCount; i++) {
            const jsprProvisioning_t *prov = &messageProvisioning->provisioning[i];

            PyObject *pyTopic = Py_BuildValue("{s:s,s:i,s:i,s:i,s:i}", 
                                                "topicName", prov->topicName,
                                                "topicId", prov->topicId,
                                                "priority", (int)prov->priority,
                                                "discardTimeSeconds", prov->discardTimeSeconds,
                                                "maxQueueDepth", prov->maxQueueDepth);

            PyList_SetItem(pyProvisioningList, i, pyTopic);
        }

        Py_INCREF(Py_True);
        Py_INCREF(Py_False);

        PyObject *pyMessageProvisioning = Py_BuildValue("{s:O,s:i,s:O}",
                                                        "provisioning", pyProvisioningList,
                                                        "topicCount", messageProvisioning->topicCount,
                                                        "provisioningSet", messageProvisioning->provisioningSet ? Py_True : Py_False);

        PyObject *result = PyObject_CallFunctionObjArgs(py_messageProvisioning_cb, pyMessageProvisioning, NULL);

        if (!result) {
            PyErr_Print();
        }
        else {
            Py_DECREF(result);
        }

        Py_XDECREF(pyMessageProvisioning);
        PyGILState_Release(gstate);
    }
}

void mo_message_complete_callback(const uint16_t id, const rbMsgStatus_t status) {

    if (py_moMessageComplete_cb && PyCallable_Check(py_moMessageComplete_cb)) {

        PyGILState_STATE gstate = PyGILState_Ensure();

        PyObject *result = PyObject_CallFunction(py_moMessageComplete_cb, "Ii", id, status);

        if (!result) {
            PyErr_Print();
        }
        else {
            Py_DECREF(result);
        }

        PyGILState_Release(gstate);
    }
}

void mt_message_complete_callback(const uint16_t id, const rbMsgStatus_t status) {

    if (py_mtMessageComplete_cb && PyCallable_Check(py_mtMessageComplete_cb)) {

        PyGILState_STATE gstate = PyGILState_Ensure();

        PyObject *result = PyObject_CallFunction(py_mtMessageComplete_cb, "Ii", id, status);

        if (!result) {
            PyErr_Print();
        }
        else {
            Py_DECREF(result);
        }

        PyGILState_Release(gstate);
    }
}

void constellation_state_callback(const jsprConstellationState_t *constellationState) {

    if (py_constellationState_cb && PyCallable_Check(py_constellationState_cb)) {

        PyGILState_STATE gstate = PyGILState_Ensure();

        Py_INCREF(Py_True);
        Py_INCREF(Py_False);

        PyObject *pyConstellationState = Py_BuildValue("{s:O,s:i,s:i}",
                                                        "constellationVisible", constellationState->constellationVisible ? Py_True : Py_False,
                                                        "signalBars", constellationState->signalBars,
                                                        "signalLevel", constellationState->signalLevel);

        PyObject *result = PyObject_CallFunctionObjArgs(py_constellationState_cb, pyConstellationState, NULL);

        if (!result) {
            PyErr_Print();
        }
        else {
            Py_DECREF(result);
        }

        Py_XDECREF(pyConstellationState);
        PyGILState_Release(gstate);
    }
}

static PyObject *py_set_message_provisioning_callback(PyObject *self, PyObject *args) {

    PyObject *cb;

    if (!PyArg_ParseTuple(args, "O", &cb))
        return NULL;

    if (!PyCallable_Check(cb)) {
        PyErr_SetString(PyExc_TypeError, "messageProvisioning must be callable");
        return NULL;
    }

    Py_XINCREF(cb);
    Py_XDECREF(py_messageProvisioning_cb);
    py_messageProvisioning_cb = cb;

    g_callbacks.messageProvisioning = message_provisioning_callback;

    rbRegisterCallbacks(&g_callbacks);

    Py_RETURN_NONE;
}

static PyObject *py_set_mo_message_complete_callback(PyObject *self, PyObject *args) {

    PyObject *cb;

    if (!PyArg_ParseTuple(args, "O", &cb))
        return NULL;

    if (!PyCallable_Check(cb)) {
        PyErr_SetString(PyExc_TypeError, "moMessageComplete must be callable");
        return NULL;
    }

    Py_XINCREF(cb);
    Py_XDECREF(py_moMessageComplete_cb);
    py_moMessageComplete_cb = cb;

    g_callbacks.moMessageComplete = mo_message_complete_callback;

    rbRegisterCallbacks(&g_callbacks);

    Py_RETURN_NONE;
}

static PyObject *py_set_mt_message_complete_callback(PyObject *self, PyObject *args) {

    PyObject *cb;

    if (!PyArg_ParseTuple(args, "O", &cb))
        return NULL;

    if (!PyCallable_Check(cb)) {
        PyErr_SetString(PyExc_TypeError, "mtMessageComplete must be callable");
        return NULL;
    }

    Py_XINCREF(cb);
    Py_XDECREF(py_mtMessageComplete_cb);
    py_mtMessageComplete_cb = cb;

    g_callbacks.mtMessageComplete = mt_message_complete_callback;

    rbRegisterCallbacks(&g_callbacks);

    Py_RETURN_NONE;
}

static PyObject *py_set_constellation_state_callback(PyObject *self, PyObject *args) {

    PyObject *cb;

    if (!PyArg_ParseTuple(args, "O", &cb))
        return NULL;

    if (!PyCallable_Check(cb)) {
        PyErr_SetString(PyExc_TypeError, "constellationState must be callable");
        return NULL;
    }

    Py_XINCREF(cb);
    Py_XDECREF(py_constellationState_cb);
    py_constellationState_cb = cb;

    g_callbacks.constellationState = constellation_state_callback;

    rbRegisterCallbacks(&g_callbacks);

    Py_RETURN_NONE;
}

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

static PyObject *py_sendMessageAsync(PyObject *self, PyObject *args) {

    int result, topic;
    char * data;
    Py_ssize_t length;

    if (!_PyArg_ParseTuple_SizeT(args, "is#", &topic, &data, &length)) {

        return NULL;

    }

    result = rbSendMessageAsync(topic, data, length);

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

static PyObject *py_receiveMessageAsync(PyObject *self, PyObject *args) {

    char* mtBuffer;

    const size_t mtLength = rbReceiveMessageAsync(&mtBuffer);

    if ((mtLength > 0) && (mtBuffer != NULL)) {

        PyObject* res = _Py_BuildValue_SizeT("y#", mtBuffer, mtLength);

        return res;

    }

    return Py_BuildValue("y", NULL);

}

static PyObject *py_rbAcknowledgeReceiveHeadAsync(PyObject *self, PyObject *args) {

    int result;

    result = rbAcknowledgeReceiveHeadAsync();

    return Py_BuildValue("i", result);

}

static PyObject *py_rbReceiveLockAsync(PyObject *self, PyObject *args) {

    rbReceiveLockAsync();
    Py_RETURN_NONE;
}

static PyObject *py_rbReceiveUnlockAsync(PyObject *self, PyObject *args) {

    rbReceiveUnlockAsync();
    Py_RETURN_NONE;
}

static PyObject *py_rbSendLockAsync(PyObject *self, PyObject *args) {

    rbSendLockAsync();
    Py_RETURN_NONE;
}

static PyObject *py_rbSendUnlockAsync(PyObject *self, PyObject *args) {

    rbSendUnlockAsync();
    Py_RETURN_NONE;
}

static PyObject *py_rbPoll(PyObject *self, PyObject *args) {

    rbPoll();
    Py_RETURN_NONE;
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
    {"send_message_async", py_sendMessageAsync, METH_VARARGS, "Function for sending mo data to any topic asynchronously"},
    {"receive_message", py_receiveMessage, METH_VARARGS, "Function for receiving mt data"},
    {"receive_message_with_topic", py_receiveMessageWithTopic, METH_VARARGS, "Function for receiving mt data from topic"},
    {"receive_message_async", py_receiveMessageAsync, METH_VARARGS, "Function for receiving mt data asynchronously"},
    {"acknowledge_receive_head_async", py_rbAcknowledgeReceiveHeadAsync, METH_VARARGS, "Function for acknowledging the head of the MT queue"},
    {"receive_lock_async", py_rbReceiveLockAsync, METH_VARARGS, "Function for locking the incoming message queue"},
    {"receive_unlock_async", py_rbReceiveUnlockAsync, METH_VARARGS, "Function for unlocking the incoming message queue"},
    {"send_lock_async", py_rbSendLockAsync, METH_VARARGS, "Function for locking the outgoing message queue"},
    {"send_unlock_async", py_rbSendUnlockAsync, METH_VARARGS, "Function for unlocking the outgoing message queue"},
    {"poll", py_rbPoll, METH_VARARGS, "Function which polls for responses from the modem for asynchronous functionality"},
    {"set_message_provisioning_callback", py_set_message_provisioning_callback, METH_VARARGS, "Function which registers the user defined provisioning callback for asynchronous functionality"},
    {"set_mo_message_complete_callback", py_set_mo_message_complete_callback, METH_VARARGS, "Function which registers the user defined mo message callback for asynchronous functionality"},
    {"set_mt_message_complete_callback", py_set_mt_message_complete_callback, METH_VARARGS, "Function which registers the user defined mt message callback for asynchronous functionality"},
    {"set_constellation_state_callback", py_set_constellation_state_callback, METH_VARARGS, "Function which registers the user defined signal level callback for asynchronous functionality"},
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
