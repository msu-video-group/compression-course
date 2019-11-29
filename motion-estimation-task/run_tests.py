import pytest
import me_estimator
import cv2
from skimage.measure import compare_ssim, compare_psnr
import numpy as np
import matplotlib.pyplot as plt


def warp_flow(img, flow_y, flow_x):
    flow = np.dstack([flow_x, flow_y])
    h, w = flow.shape[:2]
    flow[:, :, 0] += np.arange(w)
    flow[:, :, 1] += np.arange(h)[:, np.newaxis]
    result = np.zeros(img.shape, np.uint8)
    result = cv2.remap(img, flow.astype(np.float32), None, cv2.INTER_NEAREST, cv2.BORDER_REPLICATE)
    
    return result



def compensate(source_frame, me):
    of_y, of_x = me.ConvertToOF()
    print(of_y, of_x)
    return warp_flow(source_frame, of_y, of_x)


def test_equal():
    frame = cv2.imread('images/kiki.png', 0)
    me = me_estimator.MotionEstimator(448, 240, 100, False)
    result = me.Estimate(frame, frame)
    compensated_frame = result.Remap(frame)
    print(
        'SSIM {}\tPSNR {}'.format(
            compare_ssim(frame, compensated_frame, multichannel=True),
            compare_psnr(frame, compensated_frame)
        )
    )
    assert np.abs(frame - compensated_frame).sum() < 0.01


def test_equal_shift_upper():
    frame = cv2.imread('images/kiki.png', 0)
    shifted_frame = np.zeros(frame.shape, np.uint8)
    height = shifted_frame.shape[0]
    shifted_frame[0: height - 1, :] = frame[1: height, :]
    me = me_estimator.MotionEstimator(448, 240, 100, False)
    result = me.Estimate(shifted_frame, frame)
    compensated_frame = result.Remap(frame)

    print(
        'SSIM {}\tPSNR {}'.format(
            compare_ssim(shifted_frame[1: height, :], compensated_frame[1: height, :], multichannel=True),
            compare_psnr(shifted_frame[1: height, :], compensated_frame[1: height, :])
        )
    )
    assert (np.abs(shifted_frame[16: height-16, :].astype(np.int32) - compensated_frame[16: height-16, :])).sum() == 0


def test_equal_remap():
    frame = cv2.imread('images/kiki.png', 0)
    shifted_frame = np.zeros(frame.shape, np.uint8)
    height = shifted_frame.shape[0]
    shifted_frame[0: height - 1, :] = frame[1: height, :]
    me = me_estimator.MotionEstimator(448, 240, 100, False)
    result = me.Estimate(shifted_frame, frame)
    compensated_frame = result.Remap(frame)
    compensated_frame_reference = compensate(frame, result)
    print(compare_ssim(compensated_frame_reference, compensated_frame))
    print(np.abs(compensated_frame - compensated_frame_reference).mean())
    assert np.abs(compensated_frame - compensated_frame_reference).mean() < 1
