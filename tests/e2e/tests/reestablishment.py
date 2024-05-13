#
# Copyright 2021-2024 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

"""
Ping + Reestablishment Tests
"""
import logging
import time
from typing import Optional, Sequence, Union

from pytest import mark
from retina.client.manager import RetinaTestManager
from retina.launcher.artifacts import RetinaTestData
from retina.launcher.utils import configure_artifacts, param
from retina.protocol.fivegc_pb2_grpc import FiveGCStub
from retina.protocol.gnb_pb2_grpc import GNBStub
from retina.protocol.ue_pb2_grpc import UEStub

from .steps.configuration import configure_test_parameters
from .steps.stub import ping_start, ping_wait_until_finish, start_network, stop, ue_reestablishment, ue_start_and_attach


@mark.parametrize(
    "band, common_scs, bandwidth, enable_channel_noise, always_download_artifacts",
    (
        param(3, 15, 50, False, True, id="band:%s-scs:%s-bandwidth:%s-noise:%s-artifacts:%s"),
        param(41, 30, 50, False, False, id="band:%s-scs:%s-bandwidth:%s-noise:%s-artifacts:%s"),
        param(3, 15, 50, True, True, id="band:%s-scs:%s-bandwidth:%s-noise:%s-artifacts:%s"),
        param(41, 30, 50, True, False, id="band:%s-scs:%s-bandwidth:%s-noise:%s-artifacts:%s"),
    ),
)
@mark.zmq
@mark.flaky(reruns=2, only_rerun=["failed to start", "Attach timeout reached", "StatusCode.ABORTED"])
# pylint: disable=too-many-arguments
def test_zmq_reestablishment(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_8: UEStub,
    fivegc: FiveGCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    enable_channel_noise: bool,
    always_download_artifacts: bool,
):
    """
    ZMQ Ping + Reestablishment
    """

    test_duration_per_ue_sec = 50
    reestablishment_interval = 10  # seconds
    reestablishment_count = int(test_duration_per_ue_sec / reestablishment_interval)

    _ping_and_reestablishment_multi_ues(
        retina_manager=retina_manager,
        retina_data=retina_data,
        ue_array=ue_8,
        gnb=gnb,
        fivegc=fivegc,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=None,  # default from testbed
        global_timing_advance=0,
        time_alignment_calibration=0,
        always_download_artifacts=always_download_artifacts,
        enable_channel_noise=enable_channel_noise,
        reestablishment_count=reestablishment_count,
        reestablishment_interval=reestablishment_interval,
        ping_count=test_duration_per_ue_sec,
        warning_as_errors=True,
    )


# pylint: disable=too-many-arguments,too-many-locals
def _ping_and_reestablishment_multi_ues(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue_array: Sequence[UEStub],
    fivegc: FiveGCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
    sample_rate: Optional[int],
    global_timing_advance: int,
    time_alignment_calibration: Union[int, str],
    always_download_artifacts: bool,
    enable_channel_noise: bool,
    ping_count: int,
    warning_as_errors: bool = True,
    reestablishment_count: int = 1,
    reestablishment_interval: int = 3,
):
    logging.info("Reestablishment / Ping Test")

    configure_test_parameters(
        retina_manager=retina_manager,
        retina_data=retina_data,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=sample_rate,
        global_timing_advance=global_timing_advance,
        time_alignment_calibration=time_alignment_calibration,
        enable_channel_noise=enable_channel_noise,
        enable_qos_reestablishment=True,
    )

    configure_artifacts(
        retina_data=retina_data,
        always_download_artifacts=always_download_artifacts,
    )

    start_network(ue_array, gnb, fivegc)

    ue_attach_info_dict = ue_start_and_attach(ue_array, gnb, fivegc)

    for index, ue_stub in enumerate(ue_array):
        # Start pings and reestablishment
        logging.info("Starting Pings and reestablishment in background")
        ping_task_array = ping_start(ue_attach_info_dict, fivegc, ping_count)

        for _ in range(reestablishment_count):
            ue_reestablishment(ue_stub, reestablishment_interval)
            time.sleep(reestablishment_interval)

        ping_wait_until_finish(ping_task_array, index)

    ping_count_end = 20
    # Start ping in end and wait
    logging.info("Starting Pings after completed reestablishments")
    ping_wait_until_finish(ping_start(ue_attach_info_dict, fivegc, ping_count_end))

    stop(ue_array, gnb, fivegc, retina_data, warning_as_errors=warning_as_errors)
