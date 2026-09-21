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

import torch
import math
import numpy as np
def compute_mae(model, loader, device, max_ang, min_ang, max_vel, min_vel):
   model.eval()
   angle_err  = []
   vel_err    = []
   
   vel_true   = []
   vel_pred   = []
   angle_true = []
   angle_pred = []

   with torch.no_grad():
      for nb, (seg_img, mask, ctl_img, ang, vel)  in enumerate(loader, start=1):
         seg_img = seg_img.to(device=device, dtype=torch.float32)
         mask = mask.to(device=device, dtype=torch.long).squeeze(1)
         ctl_img = ctl_img.to(device=device, dtype=torch.float32)
         ang = ang.to(device=device, dtype=torch.float32)
         vel = vel.to(device=device, dtype=torch.float32)
         ctrl_target = torch.stack((ang, vel), dim=1).to(device=device)
         
         preds = model(ctl_img)

         angle_p = preds[:, 0] * (max_ang - min_ang) + min_ang
         vel_p   = preds[:, 1] * (max_vel - min_vel) + min_vel

         angle_t = ctrl_target[:, 0] * (max_ang - min_ang) + min_ang
         vel_t   = ctrl_target[:, 1] * (max_vel - min_vel) + min_vel
         
         vel_true.append(vel_t.cpu())
         vel_pred.append(vel_p.cpu())
         angle_true.append(angle_t.cpu())
         angle_pred.append(angle_p.cpu())

         angle_err.append(torch.abs(angle_t - angle_p).cpu())
         vel_err.append(torch.abs(vel_t - vel_p).cpu())
   
   
   vel_true = torch.cat(vel_true).numpy()
   vel_pred = torch.cat(vel_pred).numpy()
   
   angle_true = torch.cat(angle_true).numpy()
   angle_pred = torch.cat(angle_pred).numpy()
   
   rmse_vel = np.sqrt(np.mean((vel_true - vel_pred) ** 2))
   rmse_ang = np.sqrt(np.mean((angle_true - angle_pred) ** 2))
   
   

   angle_err = torch.cat(angle_err).numpy()
   vel_err   = torch.cat(vel_err).numpy()
   
   

   return angle_err, vel_err, rmse_vel, rmse_ang
