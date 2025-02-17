# MegEngine is Licensed under the Apache License, Version 2.0 (the "License")
#
# Copyright (c) 2014-2021 Megvii Inc. All rights reserved.
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT ARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
import importlib
import warnings
from functools import wraps

from deprecated.sphinx import deprecated

warnings.filterwarnings(action="default", module="megengine")


def deprecated_func(version, origin, name, tbd):
    r"""

    Args:
        version: version to deprecate this function
        origin: origin module path
        name: function name
        tbd: to be discussed, if true, ignore warnings
    """
    should_warning = not tbd

    def wrapper(*args, **kwargs):
        nonlocal should_warning
        module = importlib.import_module(origin)
        func = module.__getattribute__(name)
        if should_warning:
            warnings.warn(
                "Call to deprecated function {}. (use {}.{} instead) -- Deprecated since version {}.".format(
                    name, origin, name, version
                ),
                category=DeprecationWarning,
                stacklevel=2,
            )
        return func(*args, **kwargs)

    return wrapper


def deprecated_kwargs_default(version, kwargs_name, kwargs_pos):
    r"""
    Args:
        version: version to deprecate this default
        kwargs_name: kwargs name
        kwargs_pos: kwargs position
    """

    def deprecated(func):
        @wraps(func)
        def wrapper(*args, **kwargs):
            if len(args) < kwargs_pos and kwargs_name not in kwargs:
                warnings.warn(
                    "the default behavior for {} will be changed in version {}, please use it in keyword parameter way".format(
                        kwargs_name, version
                    ),
                    category=PendingDeprecationWarning,
                    stacklevel=2,
                )
            return func(*args, **kwargs)

        return wrapper

    return deprecated
