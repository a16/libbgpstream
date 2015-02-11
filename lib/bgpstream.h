/*
 * libbgpstream
 *
 * Chiara Orsini, CAIDA, UC San Diego
 * chiara@caida.org
 *
 * Copyright (C) 2013 The Regents of the University of California.
 *
 * This file is part of libbgpstream.
 *
 * libbgpstream is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libbgpstream is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libbgpstream.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __BGPSTREAM_H
#define __BGPSTREAM_H

#include <bgpstream_utils.h>
#include <bgpstream_elem.h>
#include <bgpstream_record.h>

/** @file
 *
 * @brief Header file that exposes the public interface of bgpstream. See
 * bgpstream_record.h and bgpstream_elem.h for the public interfaces of
 * bgpstream_record and bgpstream_elem respectively.
 *
 * @author Chiara Orsini
 *
 */

/**
 * @name Public Opaque Data Structures
 *
 * @{ */

/** Opaque handle that represents a BGP Stream instance */
typedef struct struct_bgpstream_t bgpstream_t;

/** @} */

/**
 * @name Public Enums
 *
 * @{ */

/** The type of the filter to be applied */
typedef enum {

  /** Filter records based on associated project (e.g. 'ris') */
  BGPSTREAM_FILTER_TYPE_PROJECT       = 1,

  /** Filter records based on collector (e.g. 'rrc01') */
  BGPSTREAM_FILTER_TYPE_COLLECTOR     = 2,

  /** Filter records based on record type (e.g. 'updates') */
  BGPSTREAM_FILTER_TYPE_RECORD_TYPE   = 3,

} bgpstream_filter_type_t;


/** Data Interface IDs */
typedef enum {

  /** MySQL data interface */
  BGPSTREAM_DATA_INTERFACE_MYSQL      = 1,

  /** Customlist interface */
  BGPSTREAM_DATA_INTERFACE_CUSTOMLIST = 2,

  /** CSV file interface */
  BGPSTREAM_DATA_INTERFACE_CSVFILE    = 3,

} bgpstream_data_interface_id_t;


/** @} */

/**
 * @name Public Data Structures
 *
 * @{ */

/** Structure that contains information about a BGP Stream Data Interface */
typedef struct struct_bgpstream_data_interface_info
{

  /** The ID of this data interface */
  bgpstream_data_interface_id_t id;

  /** The name of this data interface */
  char *name;

  /** A human-readable description of this data interface */
  char *description;

} bgpstream_data_interface_info_t;

/** Structure that represents BGP Stream Data Interface Option */
typedef struct struct_bgpstream_data_interface_option
{

  /** The ID of the data interface that this option applies to */
  bgpstream_data_interface_id_t if_id;

  /** An internal, interface-specific ID for this option */
  int id;

  /** The human-readable name of the option */
  char *name;

  /** A human-readable description of the option */
  char *description;

} bgpstream_data_interface_option_t;

/** @} */

/**
 * @name Public API Functions
 *
 * @{ */

/** Create a new BGP Stream instance
 *
 * @return a pointer to a BGP Stream instance if successful, NULL otherwise
 */
bgpstream_t *bgpstream_create();

/** Add a filter in order to select a subset of the bgp data available
 *
 * @param bs            pointer to a BGP Stream instance to filter
 * @param filter_type   the type of the filter to apply
 * @param filter_value  the value to set the filter to
 */
void bgpstream_add_filter(bgpstream_t *bs,
                          bgpstream_filter_type_t filter_type,
			  const char* filter_value);

/** Add a filter to select a specific time range from the BGP data available
 *
 * @param bs            pointer to a BGP Stream instance to filter
 * @param begin_time    the first time that will match the filter (inclusive)
 * @param end_time      the last time that will match the filter (inclusive)
 */
void bgpstream_add_interval_filter(bgpstream_t *bs,
				   uint32_t begin_time,
                                   uint32_t end_time);

/** Get a list of data interfaces that are currently supported
 *
 * @param bs            pointer to the BGP Stream instance
 * @param[out] if_cnt   pointer to a value to be set with the number of elements
 *                      in the returned array
 * @return borrowed pointer to an array of bgpstream_datasource_type_t values
 *
 * @note the returned array belongs to BGP Stream. It must not be freed by the
 * user.
 */
bgpstream_data_interface_id_t *bgpstream_get_data_interfaces(bgpstream_t *bs,
                                                             int *if_cnt);

/** Get the ID of the data interface with the given name
 *
 * @param name          name of the data interface to retrieve the ID for
 * @return the ID of the data interface with the given name, 0 if no matching
 * interface was found
 */
bgpstream_data_interface_id_t
bgpstream_get_data_interface_id_by_name(bgpstream_t *bs, const char *name);

/** Get information for the given data interface
 *
 * @param bs            pointer to a BGP Stream instance
 * @param if_id         ID of the interface to get the name for
 * @return borrowed pointer to an interface info structure
 */
bgpstream_data_interface_info_t *
bgpstream_get_data_interface_info(bgpstream_t *bs,
                                  bgpstream_data_interface_id_t if_id);

/** Get a list of valid option types for the given data interface
 *
 * @param bs            pointer to a BGP Stream instance
 * @param if_id         ID of the interface to get option names for
 * @param[out] opt_cnt  pointer to a value to be set with the number of options
 *                      in the returned array
 * @return borrowed pointer to an array of options
 *
 * @note the returned array belongs to BGP Stream. It must not be freed by the
 * user.
 */
bgpstream_data_interface_option_t *
bgpstream_get_data_interface_options(bgpstream_t *bs,
                                     bgpstream_data_interface_id_t if_id,
                                     int *opt_cnt);

/** Get the data interface option for the given data interface and option name
 *
 * @param bs            pointer to a BGP Stream instance
 * @param if_id         ID of the interface to get option info for
 * @param name          name of the option to retrieve
 * @return pointer to the option information with the given name, NULL if either
 * the interface ID is not valid, or the name does not match any options
 */
bgpstream_data_interface_option_t *
bgpstream_get_data_interface_option_by_name(bgpstream_t *bs,
                                            bgpstream_data_interface_id_t if_id,
                                            const char *name);

/** Set a data interface option
 *
 * @param bs            pointer to a BGP Stream instance to configure
 * @param option_type   pointer to the option to set
 * @param option_value  value to set the option to
 *
 * Use the bgpstream_get_data_interface_options function to discover the set of
 * options for an interface.
 */
void bgpstream_set_data_interface_option(bgpstream_t *bs,
                                bgpstream_data_interface_option_t *option_type,
                                const char *option_value);

/** Set the data interface that BGP Stream uses to find BGP data
 *
 * @param bs            pointer to a BGP Stream instance to configure
 * @param if_id         ID of the data interface to use
 */
void bgpstream_set_data_interface(bgpstream_t *bs,
                                  bgpstream_data_interface_id_t if_id);

/** Configure the interface to blocks waiting for new data instead of returning
 * end-of-stream if no more data is available.
 *
 * @param bs            pointer to a BGP Stream instance to put into blocking
 *                      mode
 */
void bgpstream_set_blocking(bgpstream_t *bs);

/** Start the given BGP Stream instance.
 *
 * @param bs            pointer to a BGP Stream instance to start
 * @return 0 if the stream was started successfully, -1 otherwise
 *
 * This function must be called after all configuration functions, and before
 * the first call to bgpstream_get_next_record.
 */
int bgpstream_start(bgpstream_t *bs);

/** Retrieve from the stream, the next record that matches configured filters.
 *
 * @param bs            pointer to a BGP Stream instance to get record from
 * @param record        pointer to a bgpstream record instance created using
 *                      bgpstream_record_create
 * @return >0 if a record was read successfully, 0 if end-of-stream has been
 * reached, <0 if an error occurred.
 *
 * The record passed to this function may be reused for subsequent calls if
 * state for previous records is not needed (i.e. the records are processed
 * independently of each other). If records are not processed independently,
 * then a new record must be created for each call to this function.
 */
int bgpstream_get_next_record(bgpstream_t *bs,
                              bgpstream_record_t *record);

/** Stop the given BGP Stream instance
 *
 * @param bs            pointer to a BGP Stream instance to stop
 */
void bgpstream_stop(bgpstream_t *bs);

/** Destroy the given BGP Stream instance
 *
 * @param bs            pointer to a BGP Stream instance to destroy
 */
void bgpstream_destroy(bgpstream_t *bs);

/** @} */

#endif /* __BGPSTREAM_H */
