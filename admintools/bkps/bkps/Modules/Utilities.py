#  This project is licensed as below.
#
#  ***************************************************************************
#
#  Copyright 2020-2024 Intel Corporation. All Rights Reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#
#  1. Redistributions of source code must retain the above copyright notice,
#  this list of conditions and the following disclaimer.
#
#  2. Redistributions in binary form must reproduce the above copyright
#  notice, this list of conditions and the following disclaimer in the
#  documentation and/or other materials provided with the distribution.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
#  PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
#  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
#  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#  ***************************************************************************

import base64
import json
import os
import sys


def verify_hex_input(usr_input):
    if usr_input and len(usr_input) > 0:
        if is_hex(usr_input):
            return usr_input
        else:
            print('Parameter should be hex encoded')


def bytes2int(data):
    return int.from_bytes(data, byteorder="big")


def int2bytes(data):
    return data.to_bytes(4, byteorder="big")


def reverse(data):
    array = bytearray()
    array.extend(data)
    array.reverse()
    return array


def read_file(input_file):
    if not os.path.isfile(input_file):
        sys.exit("File does not exist: {}".format(input_file))

    with open(input_file, encoding='utf8') as file:
        payload = file.read().strip()

    return payload


def is_hex(input_data):
    return check_number(input_data, base=16)


def contains_hex_encoded_length_one_of(input_data, lengths):
    return len(input_data) / 2 in lengths


def contains_hex_encoded_length(input_data, length):
    return len(input_data) / 2 == length


def check_number(number, base=10):
    try:
        int(number, base)
    except ValueError:
        return False
    return True


def check_number_non_negative(number):
    return check_number(number) and int(number) >= 0


def check_list_numbers(input_list):
    if len(input_list) == 1 and input_list[0] == '':
        return True

    for value in input_list:
        is_valid = check_number(value) and check_number_non_negative(value)
        if not is_valid:
            return False
    return True


def parse_list_numbers(input_list):
    new_list = []
    for value in input_list:
        try:
            new_list.append(int(value))
        except ValueError:
            continue
    return new_list


def encode_to_base64(data):
    if isinstance(data, bytes):
        binary_data = data
    else:
        binary_data = data.encode('UTF-8')

    return base64.standard_b64encode(binary_data).decode('UTF-8')


def check_file(input_file):
    if not os.path.isfile(input_file):
        sys.exit("File does not exist: {}".format(input_file))


def check_file_soft(input_file, with_warning=False):
    if not os.path.isfile(input_file):
        if with_warning:
            print("File does not exist: {}".format(input_file))
        return False
    else:
        return True


def show_output(output):
    if is_json(output):
        parsed = json.loads(output)
        print(json.dumps(parsed, indent=2, sort_keys=False))
    else:
        print(output)


def is_json(myjson):
    try:
        json.loads(myjson)
    except ValueError as e:
        return False
    return True


def is_command_docopt(valid_main_categories, arguments):
    return True in [arguments.get(opt) for opt in valid_main_categories]


def input_string(label, not_empty):
    while True:
        usr_input = input('Enter {}: '.format(label))
        if usr_input:
            if not_empty:
                if len(usr_input) > 0:
                    return usr_input
            else:
                return usr_input


def input_number(label, default=None):
    while True:
        if default is None:
            usr_input = input('Enter {}: '.format(label))
            if usr_input and len(usr_input) > 0 and check_number_non_negative(usr_input):
                return int(usr_input)
        else:
            usr_input = input('Enter {} (default: {}): '.format(label, str(default)))
            if usr_input and len(usr_input) > 0 and check_number_non_negative(usr_input):
                return int(usr_input)
            elif not usr_input or len(usr_input) == 0:
                return default


def input_file_path(label):
    while True:
        usr_input = input('Enter path to {}: '.format(label))
        if check_file_soft(usr_input, True):
            return usr_input


def input_debug_mode():
    while True:
        usr_input = input('Enable debug mode (Y/N): ')
        to_verify = usr_input.upper()
        if to_verify in ('Y', 'N'):
            return to_verify == 'Y'


def input_using_proxy():
    while True:
        usr_input = input('Use system proxy (Y/N): ')
        to_verify = usr_input.upper()
        if to_verify in ('Y', 'N'):
            return to_verify == 'Y'


def input_config_type():
    while True:
        usr_input = input('Use config file (Y) or run in interactive mode (N): ')
        to_verify = usr_input.upper()
        if to_verify in ('Y', 'N'):
            return to_verify == 'Y'


def split_array(data):
    if data is None:
        return []
    else:
        return data.split(',')


def input_req_configuration_import_types():
    while True:
        usr_input = input('Create or update configuration using: ([J]son/[I]nteractive): ')
        to_verify = usr_input.upper()
        if to_verify in ('J', 'I'):
            if to_verify == 'J':
                return '--json'
            elif to_verify == 'I':
                return '--interactive'


def assign_params_value(params, name, variable=None, optional_input_value_label=None, optional_input_value=None, type=None):
    is_valid = False
    if variable is not None:
        if variable == name:
            params[name] = True
            is_valid = True
    else:
        params[name] = True
        is_valid = True

    if is_valid:
        if optional_input_value_label is not None and optional_input_value is not None:
            if type is None:
                params[optional_input_value] = input('{}: '.format(optional_input_value_label))
            elif type == 'FILE':
                params[optional_input_value] = input_file_path(optional_input_value_label)
            elif type == 'NUMBER':
                params[optional_input_value] = input_number(optional_input_value_label)

    return is_valid


def is_hex_x_bytes_long(data, length):
    if not is_hex(data) or not contains_hex_encoded_length(data, length):
        return False
    return True

