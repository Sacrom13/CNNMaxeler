/**\file */
#ifndef SLIC_DECLARATIONS_BLOCK0_H
#define SLIC_DECLARATIONS_BLOCK0_H
#include "MaxSLiCInterface.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define CNNDFE_PCIE_ALIGNMENT (16)


/*----------------------------------------------------------------------------*/
/*---------------------------- Interface MemWrite ----------------------------*/
/*----------------------------------------------------------------------------*/




/**
 * \brief Basic static function for the interface 'MemWrite'.
 * 
 * \param [in] param_Size Interface Parameter "Size".
 * \param [in] param_Start Interface Parameter "Start".
 * \param [in] instream_DataIn The stream should be of size (param_Size * 8) bytes.
 */
void CNNDFE_MemWrite(
	int32_t param_Size,
	int32_t param_Start,
	const double *instream_DataIn);

/**
 * \brief Basic static non-blocking function for the interface 'MemWrite'.
 * 
 * Schedule to run on an engine and return immediately.
 * The status of the run can be checked either by ::max_wait or ::max_nowait;
 * note that one of these *must* be called, so that associated memory can be released.
 * 
 * 
 * \param [in] param_Size Interface Parameter "Size".
 * \param [in] param_Start Interface Parameter "Start".
 * \param [in] instream_DataIn The stream should be of size (param_Size * 8) bytes.
 * \return A handle on the execution status, or NULL in case of error.
 */
max_run_t *CNNDFE_MemWrite_nonblock(
	int32_t param_Size,
	int32_t param_Start,
	const double *instream_DataIn);

/**
 * \brief Advanced static interface, structure for the engine interface 'MemWrite'
 * 
 */
typedef struct { 
	int32_t param_Size; /**<  [in] Interface Parameter "Size". */
	int32_t param_Start; /**<  [in] Interface Parameter "Start". */
	const double *instream_DataIn; /**<  [in] The stream should be of size (param_Size * 8) bytes. */
} CNNDFE_MemWrite_actions_t;

/**
 * \brief Advanced static function for the interface 'MemWrite'.
 * 
 * \param [in] engine The engine on which the actions will be executed.
 * \param [in,out] interface_actions Actions to be executed.
 */
void CNNDFE_MemWrite_run(
	max_engine_t *engine,
	CNNDFE_MemWrite_actions_t *interface_actions);

/**
 * \brief Advanced static non-blocking function for the interface 'MemWrite'.
 *
 * Schedule the actions to run on the engine and return immediately.
 * The status of the run can be checked either by ::max_wait or ::max_nowait;
 * note that one of these *must* be called, so that associated memory can be released.
 *
 * 
 * \param [in] engine The engine on which the actions will be executed.
 * \param [in] interface_actions Actions to be executed.
 * \return A handle on the execution status of the actions, or NULL in case of error.
 */
max_run_t *CNNDFE_MemWrite_run_nonblock(
	max_engine_t *engine,
	CNNDFE_MemWrite_actions_t *interface_actions);

/**
 * \brief Group run advanced static function for the interface 'MemWrite'.
 * 
 * \param [in] group Group to use.
 * \param [in,out] interface_actions Actions to run.
 *
 * Run the actions on the first device available in the group.
 */
void CNNDFE_MemWrite_run_group(max_group_t *group, CNNDFE_MemWrite_actions_t *interface_actions);

/**
 * \brief Group run advanced static non-blocking function for the interface 'MemWrite'.
 * 
 *
 * Schedule the actions to run on the first device available in the group and return immediately.
 * The status of the run must be checked with ::max_wait. 
 * Note that use of ::max_nowait is prohibited with non-blocking running on groups:
 * see the ::max_run_group_nonblock documentation for more explanation.
 *
 * \param [in] group Group to use.
 * \param [in] interface_actions Actions to run.
 * \return A handle on the execution status of the actions, or NULL in case of error.
 */
max_run_t *CNNDFE_MemWrite_run_group_nonblock(max_group_t *group, CNNDFE_MemWrite_actions_t *interface_actions);

/**
 * \brief Array run advanced static function for the interface 'MemWrite'.
 * 
 * \param [in] engarray The array of devices to use.
 * \param [in,out] interface_actions The array of actions to run.
 *
 * Run the array of actions on the array of engines.  The length of interface_actions
 * must match the size of engarray.
 */
void CNNDFE_MemWrite_run_array(max_engarray_t *engarray, CNNDFE_MemWrite_actions_t *interface_actions[]);

/**
 * \brief Array run advanced static non-blocking function for the interface 'MemWrite'.
 * 
 *
 * Schedule to run the array of actions on the array of engines, and return immediately.
 * The length of interface_actions must match the size of engarray.
 * The status of the run can be checked either by ::max_wait or ::max_nowait;
 * note that one of these *must* be called, so that associated memory can be released.
 *
 * \param [in] engarray The array of devices to use.
 * \param [in] interface_actions The array of actions to run.
 * \return A handle on the execution status of the actions, or NULL in case of error.
 */
max_run_t *CNNDFE_MemWrite_run_array_nonblock(max_engarray_t *engarray, CNNDFE_MemWrite_actions_t *interface_actions[]);

/**
 * \brief Converts a static-interface action struct into a dynamic-interface max_actions_t struct.
 *
 * Note that this is an internal utility function used by other functions in the static interface.
 *
 * \param [in] maxfile The maxfile to use.
 * \param [in] interface_actions The interface-specific actions to run.
 * \return The dynamic-interface actions to run, or NULL in case of error.
 */
max_actions_t* CNNDFE_MemWrite_convert(max_file_t *maxfile, CNNDFE_MemWrite_actions_t *interface_actions);



/*----------------------------------------------------------------------------*/
/*---------------------------- Interface MemRead -----------------------------*/
/*----------------------------------------------------------------------------*/




/**
 * \brief Basic static function for the interface 'MemRead'.
 * 
 * \param [in] param_Size Interface Parameter "Size".
 * \param [in] param_Start Interface Parameter "Start".
 * \param [out] outstream_DataOut The stream should be of size (param_Size * 8) bytes.
 */
void CNNDFE_MemRead(
	int32_t param_Size,
	int32_t param_Start,
	double *outstream_DataOut);

/**
 * \brief Basic static non-blocking function for the interface 'MemRead'.
 * 
 * Schedule to run on an engine and return immediately.
 * The status of the run can be checked either by ::max_wait or ::max_nowait;
 * note that one of these *must* be called, so that associated memory can be released.
 * 
 * 
 * \param [in] param_Size Interface Parameter "Size".
 * \param [in] param_Start Interface Parameter "Start".
 * \param [out] outstream_DataOut The stream should be of size (param_Size * 8) bytes.
 * \return A handle on the execution status, or NULL in case of error.
 */
max_run_t *CNNDFE_MemRead_nonblock(
	int32_t param_Size,
	int32_t param_Start,
	double *outstream_DataOut);

/**
 * \brief Advanced static interface, structure for the engine interface 'MemRead'
 * 
 */
typedef struct { 
	int32_t param_Size; /**<  [in] Interface Parameter "Size". */
	int32_t param_Start; /**<  [in] Interface Parameter "Start". */
	double *outstream_DataOut; /**<  [out] The stream should be of size (param_Size * 8) bytes. */
} CNNDFE_MemRead_actions_t;

/**
 * \brief Advanced static function for the interface 'MemRead'.
 * 
 * \param [in] engine The engine on which the actions will be executed.
 * \param [in,out] interface_actions Actions to be executed.
 */
void CNNDFE_MemRead_run(
	max_engine_t *engine,
	CNNDFE_MemRead_actions_t *interface_actions);

/**
 * \brief Advanced static non-blocking function for the interface 'MemRead'.
 *
 * Schedule the actions to run on the engine and return immediately.
 * The status of the run can be checked either by ::max_wait or ::max_nowait;
 * note that one of these *must* be called, so that associated memory can be released.
 *
 * 
 * \param [in] engine The engine on which the actions will be executed.
 * \param [in] interface_actions Actions to be executed.
 * \return A handle on the execution status of the actions, or NULL in case of error.
 */
max_run_t *CNNDFE_MemRead_run_nonblock(
	max_engine_t *engine,
	CNNDFE_MemRead_actions_t *interface_actions);

/**
 * \brief Group run advanced static function for the interface 'MemRead'.
 * 
 * \param [in] group Group to use.
 * \param [in,out] interface_actions Actions to run.
 *
 * Run the actions on the first device available in the group.
 */
void CNNDFE_MemRead_run_group(max_group_t *group, CNNDFE_MemRead_actions_t *interface_actions);

/**
 * \brief Group run advanced static non-blocking function for the interface 'MemRead'.
 * 
 *
 * Schedule the actions to run on the first device available in the group and return immediately.
 * The status of the run must be checked with ::max_wait. 
 * Note that use of ::max_nowait is prohibited with non-blocking running on groups:
 * see the ::max_run_group_nonblock documentation for more explanation.
 *
 * \param [in] group Group to use.
 * \param [in] interface_actions Actions to run.
 * \return A handle on the execution status of the actions, or NULL in case of error.
 */
max_run_t *CNNDFE_MemRead_run_group_nonblock(max_group_t *group, CNNDFE_MemRead_actions_t *interface_actions);

/**
 * \brief Array run advanced static function for the interface 'MemRead'.
 * 
 * \param [in] engarray The array of devices to use.
 * \param [in,out] interface_actions The array of actions to run.
 *
 * Run the array of actions on the array of engines.  The length of interface_actions
 * must match the size of engarray.
 */
void CNNDFE_MemRead_run_array(max_engarray_t *engarray, CNNDFE_MemRead_actions_t *interface_actions[]);

/**
 * \brief Array run advanced static non-blocking function for the interface 'MemRead'.
 * 
 *
 * Schedule to run the array of actions on the array of engines, and return immediately.
 * The length of interface_actions must match the size of engarray.
 * The status of the run can be checked either by ::max_wait or ::max_nowait;
 * note that one of these *must* be called, so that associated memory can be released.
 *
 * \param [in] engarray The array of devices to use.
 * \param [in] interface_actions The array of actions to run.
 * \return A handle on the execution status of the actions, or NULL in case of error.
 */
max_run_t *CNNDFE_MemRead_run_array_nonblock(max_engarray_t *engarray, CNNDFE_MemRead_actions_t *interface_actions[]);

/**
 * \brief Converts a static-interface action struct into a dynamic-interface max_actions_t struct.
 *
 * Note that this is an internal utility function used by other functions in the static interface.
 *
 * \param [in] maxfile The maxfile to use.
 * \param [in] interface_actions The interface-specific actions to run.
 * \return The dynamic-interface actions to run, or NULL in case of error.
 */
max_actions_t* CNNDFE_MemRead_convert(max_file_t *maxfile, CNNDFE_MemRead_actions_t *interface_actions);



/*----------------------------------------------------------------------------*/
/*------------------------ Interface RunForwardBlock0 ------------------------*/
/*----------------------------------------------------------------------------*/




/**
 * \brief Basic static function for the interface 'RunForwardBlock0'.
 * 
 * \param [in] param_InputOffset Interface Parameter "InputOffset".
 * \param [in] param_FirstOutputs Interface Parameter array FirstOutputs[] should be of size 4.
 * \param [in] param_MemControl Interface Parameter array MemControl[] should be of size 4.
 * \param [in] param_Weights Interface Parameter array Weights[] should be of size 864.
 */
void CNNDFE_RunForwardBlock0(
	uint32_t param_InputOffset,
	const uint32_t *param_FirstOutputs,
	const uint32_t *param_MemControl,
	const double *param_Weights);

/**
 * \brief Basic static non-blocking function for the interface 'RunForwardBlock0'.
 * 
 * Schedule to run on an engine and return immediately.
 * The status of the run can be checked either by ::max_wait or ::max_nowait;
 * note that one of these *must* be called, so that associated memory can be released.
 * 
 * 
 * \param [in] param_InputOffset Interface Parameter "InputOffset".
 * \param [in] param_FirstOutputs Interface Parameter array FirstOutputs[] should be of size 4.
 * \param [in] param_MemControl Interface Parameter array MemControl[] should be of size 4.
 * \param [in] param_Weights Interface Parameter array Weights[] should be of size 864.
 * \return A handle on the execution status, or NULL in case of error.
 */
max_run_t *CNNDFE_RunForwardBlock0_nonblock(
	uint32_t param_InputOffset,
	const uint32_t *param_FirstOutputs,
	const uint32_t *param_MemControl,
	const double *param_Weights);

/**
 * \brief Advanced static interface, structure for the engine interface 'RunForwardBlock0'
 * 
 */
typedef struct { 
	uint32_t param_InputOffset; /**<  [in] Interface Parameter "InputOffset". */
	const uint32_t *param_FirstOutputs; /**<  [in] Interface Parameter array FirstOutputs[] should be of size 4. */
	const uint32_t *param_MemControl; /**<  [in] Interface Parameter array MemControl[] should be of size 4. */
	const double *param_Weights; /**<  [in] Interface Parameter array Weights[] should be of size 864. */
} CNNDFE_RunForwardBlock0_actions_t;

/**
 * \brief Advanced static function for the interface 'RunForwardBlock0'.
 * 
 * \param [in] engine The engine on which the actions will be executed.
 * \param [in,out] interface_actions Actions to be executed.
 */
void CNNDFE_RunForwardBlock0_run(
	max_engine_t *engine,
	CNNDFE_RunForwardBlock0_actions_t *interface_actions);

/**
 * \brief Advanced static non-blocking function for the interface 'RunForwardBlock0'.
 *
 * Schedule the actions to run on the engine and return immediately.
 * The status of the run can be checked either by ::max_wait or ::max_nowait;
 * note that one of these *must* be called, so that associated memory can be released.
 *
 * 
 * \param [in] engine The engine on which the actions will be executed.
 * \param [in] interface_actions Actions to be executed.
 * \return A handle on the execution status of the actions, or NULL in case of error.
 */
max_run_t *CNNDFE_RunForwardBlock0_run_nonblock(
	max_engine_t *engine,
	CNNDFE_RunForwardBlock0_actions_t *interface_actions);

/**
 * \brief Group run advanced static function for the interface 'RunForwardBlock0'.
 * 
 * \param [in] group Group to use.
 * \param [in,out] interface_actions Actions to run.
 *
 * Run the actions on the first device available in the group.
 */
void CNNDFE_RunForwardBlock0_run_group(max_group_t *group, CNNDFE_RunForwardBlock0_actions_t *interface_actions);

/**
 * \brief Group run advanced static non-blocking function for the interface 'RunForwardBlock0'.
 * 
 *
 * Schedule the actions to run on the first device available in the group and return immediately.
 * The status of the run must be checked with ::max_wait. 
 * Note that use of ::max_nowait is prohibited with non-blocking running on groups:
 * see the ::max_run_group_nonblock documentation for more explanation.
 *
 * \param [in] group Group to use.
 * \param [in] interface_actions Actions to run.
 * \return A handle on the execution status of the actions, or NULL in case of error.
 */
max_run_t *CNNDFE_RunForwardBlock0_run_group_nonblock(max_group_t *group, CNNDFE_RunForwardBlock0_actions_t *interface_actions);

/**
 * \brief Array run advanced static function for the interface 'RunForwardBlock0'.
 * 
 * \param [in] engarray The array of devices to use.
 * \param [in,out] interface_actions The array of actions to run.
 *
 * Run the array of actions on the array of engines.  The length of interface_actions
 * must match the size of engarray.
 */
void CNNDFE_RunForwardBlock0_run_array(max_engarray_t *engarray, CNNDFE_RunForwardBlock0_actions_t *interface_actions[]);

/**
 * \brief Array run advanced static non-blocking function for the interface 'RunForwardBlock0'.
 * 
 *
 * Schedule to run the array of actions on the array of engines, and return immediately.
 * The length of interface_actions must match the size of engarray.
 * The status of the run can be checked either by ::max_wait or ::max_nowait;
 * note that one of these *must* be called, so that associated memory can be released.
 *
 * \param [in] engarray The array of devices to use.
 * \param [in] interface_actions The array of actions to run.
 * \return A handle on the execution status of the actions, or NULL in case of error.
 */
max_run_t *CNNDFE_RunForwardBlock0_run_array_nonblock(max_engarray_t *engarray, CNNDFE_RunForwardBlock0_actions_t *interface_actions[]);

/**
 * \brief Converts a static-interface action struct into a dynamic-interface max_actions_t struct.
 *
 * Note that this is an internal utility function used by other functions in the static interface.
 *
 * \param [in] maxfile The maxfile to use.
 * \param [in] interface_actions The interface-specific actions to run.
 * \return The dynamic-interface actions to run, or NULL in case of error.
 */
max_actions_t* CNNDFE_RunForwardBlock0_convert(max_file_t *maxfile, CNNDFE_RunForwardBlock0_actions_t *interface_actions);

/**
 * \brief Initialise a maxfile.
 */
max_file_t* CNNDFE_init(void);

/* Error handling functions */
int CNNDFE_has_errors(void);
const char* CNNDFE_get_errors(void);
void CNNDFE_clear_errors(void);
/* Free statically allocated maxfile data */
void CNNDFE_free(void);
/* returns: -1 = error running command; 0 = no error reported */
int CNNDFE_simulator_start(void);
/* returns: -1 = error running command; 0 = no error reported */
int CNNDFE_simulator_stop(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

