#******************************************************************************
# \file
#
# $Id:$
#
# This file is part of software developed by:  Universidad de Guadalajara; Centro Universitario de Los Valles; Centro de Investigación en Procesamiento de Señales Digitales
#
# Author: Alfredo Chávez Plascencia  acplascencia00@gmail.com
# Author: Himer Avila George         himer.avila@academicos.udg.mx
#
# This file is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This file is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this file.  If not, see <http://www.gnu.org/licenses/>.
#/
import os
import matplotlib.pyplot as plt


def plots(metrics):
    root_fig = "/path/to/your/root/working/directory"
    #=============================
    # --- Plot Control metrics ---
    #=============================
    FONT_TITLE  = 22
    FONT_LABEL  = 22
    FONT_TICKS  = 16
    FONT_LEGEND = 22

    #==================
    # --- TRAIN MAE ---
    #==================
    train_mae_ang   = metrics["train_ang_err"].mean() 
    train_mae_vel   = metrics["train_vel_err"].mean()
    
    
    
    print("\n")
    print("FINAL TRAIN MAE Velocity:", train_mae_vel)
    print("FINAL TRAIN MAE Angle:", train_mae_ang)
    print("FINAL TRAIN RMSE Velocity:", metrics["train_rmse_vel"])
    print("FINAL TRAIN MAE Angle:", metrics["train_rmse_ang"])
    print("\n")
    
    # ========================================
    # --- TRAIN VELOCITY, ANGLE ERROR PLOT ---
    # ========================================
    plt.figure(figsize=(10,4))
    plt.subplot(1,2,1)
    plt.plot(metrics["train_vel_err"], 'g.-', label='Velocity Error')
    plt.xlabel("Sample index", fontsize=FONT_LABEL)
    plt.ylabel("Velocity Error", fontsize=FONT_LABEL)
    plt.title('Train velocity error', fontsize=FONT_LABEL)
    plt.ylim(0, 255)   
    plt.legend(fontsize=FONT_LEGEND)

    plt.xticks(fontsize=FONT_TICKS)
    plt.yticks(fontsize=FONT_TICKS)
    #plt.savefig("velocity_error.png", dpi=300)

    plt.subplot(1,2,2)
    plt.plot(metrics["train_ang_err"], 'r.-', label='Angle Error')
    plt.xlabel("Sample index", fontsize=FONT_LABEL)
    plt.ylabel("Angle Error", fontsize=FONT_LABEL)
    plt.title('Train angle error', fontsize=FONT_LABEL)
    plt.ylim(0, 50)   
    plt.legend(fontsize=FONT_LEGEND)

    plt.xticks(fontsize=FONT_TICKS)
    plt.yticks(fontsize=FONT_TICKS)
    #plt.savefig("train_velocity_angle_error.png", dpi=300)
    plt.savefig(root_fig + "/Figures/train_velocity_angle_error.eps", dpi=300)
    plt.savefig(root_fig + "/Figures/train_velocity_angle_error.pdf", dpi=300)
    plt.show()

    #=====================
    # --- VAL MAE ---
    #=====================
    
    val_mae_ang   = metrics["val_ang_err"].mean() 
    val_mae_vel   = metrics["val_vel_err"].mean()
    
    print("\n")
    print("FINAL VAL MAE Velocity:", val_mae_vel)
    print("FINAL VAL MAE Angle:", val_mae_ang)
    print("FINAL VAL RMSE Velocity:", metrics["val_rmse_vel"])
    print("FINAL VAL MAE Angle:", metrics["val_rmse_ang"])
    print("\n")
    
    # ========================================
    # --- VAL VELOCITY, ANGLE ERROR PLOT ---
    # ========================================
    plt.figure(figsize=(10,4))
    plt.subplot(1,2,1)
    plt.plot(metrics["val_vel_err"], 'g.-', label='Velocity Error')
    plt.xlabel("Sample index", fontsize=FONT_LABEL)
    plt.ylabel("Velocity Error", fontsize=FONT_LABEL)
    plt.title('Val velocity error', fontsize=FONT_LABEL)
    plt.ylim(0, 255) 
    plt.legend(fontsize=FONT_LEGEND)

    plt.xticks(fontsize=FONT_TICKS)
    plt.yticks(fontsize=FONT_TICKS)
    #plt.savefig("velocity_error.png", dpi=300)

    plt.subplot(1,2,2)
    plt.plot(metrics["train_ang_err"], 'r.-', label='Angle Error')
    plt.xlabel("Sample index", fontsize=FONT_LABEL)
    plt.ylabel("Angle Error", fontsize=FONT_LABEL)
    plt.title('Val angle error', fontsize=FONT_LABEL)
    plt.ylim(0, 50) 
    plt.legend(fontsize=FONT_LEGEND)

    plt.xticks(fontsize=FONT_TICKS)
    plt.yticks(fontsize=FONT_TICKS)
    #plt.savefig("val_velocity_angle_error.png", dpi=300)
    plt.savefig(root_fig + "/Figures/val_velocity_angle_error.eps", dpi=300)
    plt.savefig(root_fig + "/Figures/val_velocity_angle_error.pdf", dpi=300)
    plt.show()

    #=====================
    # --- TEST MAE ---
    #=====================
    test_mae_ang   = metrics["test_ang_err"].mean() 
    test_mae_vel   = metrics["test_vel_err"].mean()
    
    print("\n")
    print("FINAL TEST MAE Velocity:", test_mae_vel)
    print("FINAL TEST MAE Angle:", test_mae_ang)
    print("FINAL TEST RMSE Velocity:", metrics["test_rmse_vel"])
    print("FINAL TEST MAE Angle:", metrics["test_rmse_ang"])
    print("\n")
    
    # ========================================
    # --- TEST VELOCITY, ANGLE ERROR PLOT ---
    # ========================================
    plt.figure(figsize=(10,4))
    plt.subplot(1,2,1)
    plt.plot(metrics["test_vel_err"], 'g.-', label='Velocity Error')
    plt.xlabel("Sample index", fontsize=FONT_LABEL)
    plt.ylabel("Velocity Error", fontsize=FONT_LABEL)
    plt.title('Test velocity error', fontsize=FONT_LABEL)
    plt.ylim(0, 255) 
    plt.legend(fontsize=FONT_LEGEND)

    plt.xticks(fontsize=FONT_TICKS)
    plt.yticks(fontsize=FONT_TICKS)
    #plt.savefig("velocity_error.png", dpi=300)

    plt.subplot(1,2,2)
    plt.plot(metrics["test_ang_err"], 'r.-', label='Angle Error')
    plt.xlabel("Sample index", fontsize=FONT_LABEL)
    plt.ylabel("Angle Error", fontsize=FONT_LABEL)
    plt.title('Test angle error', fontsize=FONT_LABEL)
    plt.ylim(0, 50) 
    plt.legend(fontsize=FONT_LEGEND)

    plt.xticks(fontsize=FONT_TICKS)
    plt.yticks(fontsize=FONT_TICKS)
    #plt.savefig("test_velocity_angle_error.png", dpi=300)
    plt.savefig(root_fig + "/Figures/test_velocity_angle_error.eps", dpi=300)
    plt.savefig(root_fig + "/Figures/test_velocity_angle_error.pdf", dpi=300)
    plt.show()
    
    #==========================
    # --- Plot UNET metrics ---
    #==========================
    plt.figure()
    plt.plot(metrics["train_dice"], label="train")
    plt.plot(metrics["val_dice"], label="val")
    plt.plot(metrics["test_dice"], label="test")
    plt.xlabel("Epoch", fontsize=FONT_LABEL)
    plt.title("Dice Score, fontsize=FONT_LABEL")
    plt.legend(fontsize=FONT_LEGEND)

    plt.xticks(fontsize=FONT_TICKS)
    plt.yticks(fontsize=FONT_TICKS)
    #plt.savefig("train_val_test_dice.png", dpi=300)
    plt.savefig(root_fig + "/Figures/train_val_test_dice.eps", dpi=300)
    plt.savefig(root_fig + "/Figures/train_val_test_dice.pdf", dpi=300)
    plt.show()

    plt.figure()
    plt.plot(metrics["train_iou"], label="train")
    plt.plot(metrics["val_iou"], label="val")
    plt.plot(metrics["test_iou"], label="test")
    plt.xlabel("Epoch", fontsize=FONT_LABEL)
    plt.title("IoU", fontsize=FONT_LABEL)
    plt.legend(fontsize=FONT_LEGEND)

    plt.xticks(fontsize=FONT_TICKS)
    plt.yticks(fontsize=FONT_TICKS)
    #plt.savefig("train_val_test_iou.png", dpi=300)
    plt.savefig(root_fig + "/Figures/train_val_test_iou.eps", dpi=300)
    plt.savefig(root_fig + "/Figures/train_val_test_iou.pdf", dpi=300)
    plt.show()
    
    
    
    
    
    plt.figure()
    plt.plot(metrics["train_miou"], label="train")
    plt.plot(metrics["val_miou"], label="val")
    plt.plot(metrics["test_miou"], label="test")
    plt.xlabel("Epoch", fontsize=FONT_LABEL)
    plt.title("mIoU", fontsize=FONT_LABEL)
    plt.legend(fontsize=FONT_LEGEND)

    plt.xticks(fontsize=FONT_TICKS)
    plt.yticks(fontsize=FONT_TICKS)
    plt.savefig("train_val_test_miou.pdf", dpi=300)
    plt.savefig("train_val_test_miou.eps", dpi=300)
    plt.savefig(root_fig + "/Figures/train_val_test_miou.eps", dpi=300)
    plt.savefig(root_fig + "/Figures/train_val_test_miou.pdf", dpi=300)
    plt.show()
    
    plt.figure()
    plt.plot(metrics["train_mPA"], label="train")
    plt.plot(metrics["val_mPA"], label="val")
    plt.plot(metrics["test_mPA"], label="test")
    plt.xlabel("Epoch",fontsize=FONT_LABEL)
    plt.title("mAP",fontsize=FONT_LABEL)
    plt.legend(fontsize=FONT_LEGEND)

    plt.xticks(fontsize=FONT_TICKS)
    plt.yticks(fontsize=FONT_TICKS)
    plt.savefig("train_val_test_mAP.pdf", dpi=300)
    plt.savefig("train_val_test_mAP.eps", dpi=300)
    plt.savefig(root_fig + "/Figures/train_val_test_mAP.eps", dpi=300)
    plt.savefig(root_fig + "/Figures/train_val_test_mAP.pdf", dpi=300)
    plt.show()
    
    
    
    
    

    plt.figure()
    plt.plot(metrics["train_pix_acc"], label="train")
    plt.plot(metrics["val_pix_acc"], label="val")
    plt.plot(metrics["test_pix_acc"], label="test")
    plt.xlabel("Epoch", fontsize=FONT_LABEL)
    plt.title("Pixel Accuracy", fontsize=FONT_LABEL)
    plt.legend(fontsize=FONT_LEGEND)

    plt.xticks(fontsize=FONT_TICKS)
    plt.yticks(fontsize=FONT_TICKS)
    #plt.savefig("train_val_test_pix_acc.png", dpi=300)
    plt.savefig(root_fig + "/Figures/train_val_test_pix_acc.eps", dpi=300)
    plt.savefig(root_fig + "/Figures/train_val_test_pix_acc.pdf", dpi=300)
    plt.show()
    
    
    # =======================
    # --- LOSS CONTROL CURVE PLOT ---
    # =======================
    plt.figure(figsize=(8, 5))
    plt.plot(metrics["train_losses"], label="Train Loss")
    plt.plot(metrics["val_losses"], label="Val Loss")
    plt.plot(metrics["test_losses"], label="Test Loss")
    plt.xlabel("Epoch", fontsize=FONT_LABEL)
    plt.ylabel("MSE Loss", fontsize=FONT_LABEL)
    plt.title("Training & Validation Loss", fontsize=FONT_LABEL)
    #plt.ylim(0, 0.4) 
    plt.legend(fontsize=FONT_LEGEND)

    plt.xticks(fontsize=FONT_TICKS)
    plt.yticks(fontsize=FONT_TICKS)
    plt.grid()
    plt.tight_layout()
    plt.savefig("control_loss.pdf", dpi=300)
    plt.savefig("control_loss.eps", dpi=300)
    plt.savefig(root_fig + "/Figures/control_loss.eps", dpi=300)
    plt.savefig(root_fig + "/Figures/control_loss.pdf", dpi=300)
    plt.show()
    
    
    # =======================
    # --- LOSS SEG CURVE PLOT ---
    # =======================
    plt.figure(figsize=(8, 5))
    plt.plot(metrics["train_seg_losses"], label="Train Loss")
    plt.plot(metrics["val_seg_losses"], label="Val Loss")
    plt.plot(metrics["test_seg_losses"], label="Val Loss")
    plt.xlabel("Epoch", fontsize=FONT_LABEL)
    plt.ylabel("Cross Entropy Loss", fontsize=FONT_LABEL)
    plt.title("U-Net Loss", fontsize=FONT_LABEL)
    #plt.ylim(0, 0.4) 
    plt.legend(fontsize=FONT_LEGEND)

    plt.xticks(fontsize=FONT_TICKS)
    plt.yticks(fontsize=FONT_TICKS)
    plt.grid()
    plt.tight_layout()
    plt.savefig("seg_loss.pdf", dpi=300)
    plt.savefig("seg_loss.eps", dpi=300)
    plt.savefig(root_fig + "/Figures/seg_loss.eps", dpi=300)
    plt.savefig(root_fig + "/Figures/seg_loss.pdf", dpi=300)
    plt.show()
    
  
