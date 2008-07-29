/* ----------------------------------------------------------------- */
/*           The HMM-Based Speech Synthesis System (HTS)             */
/*           hts_engine API developed by HTS Working Group           */
/*           http://hts-engine.sourceforge.net/                      */
/* ----------------------------------------------------------------- */
/*                                                                   */
/*  Copyright (c) 2001-2008  Nagoya Institute of Technology          */
/*                           Department of Computer Science          */
/*                                                                   */
/*                2001-2008  Tokyo Institute of Technology           */
/*                           Interdisciplinary Graduate School of    */
/*                           Science and Engineering                 */
/*                                                                   */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/* - Redistributions of source code must retain the above copyright  */
/*   notice, this list of conditions and the following disclaimer.   */
/* - Redistributions in binary form must reproduce the above         */
/*   copyright notice, this list of conditions and the following     */
/*   disclaimer in the documentation and/or other materials provided */
/*   with the distribution.                                          */
/* - Neither the name of the HTS working group nor the names of its  */
/*   contributors may be used to endorse or promote products derived */
/*   from this software without specific prior written permission.   */
/*                                                                   */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND            */
/* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,       */
/* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF          */
/* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE          */
/* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS */
/* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,          */
/* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED   */
/* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     */
/* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON */
/* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,   */
/* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY    */
/* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE           */
/* POSSIBILITY OF SUCH DAMAGE.                                       */
/* ----------------------------------------------------------------- */

/* hts_engine libraries */
#include "HTS_hidden.h"

/* HTS_SStreamSet_initialize: initialize state stream set */
void HTS_SStreamSet_initialize(HTS_SStreamSet * sss)
{
   sss->nstream = 0;
   sss->nstate = 0;
   sss->sstream = NULL;
   sss->duration = NULL;
   sss->total_state = 0;
   sss->total_frame = 0;
}

/* HTS_SStreamSet_create: parse label and determine state duration */
void HTS_SStreamSet_create(HTS_SStreamSet * sss, HTS_ModelSet * ms,
                           HTS_Label * label, double *duration_iw,
                           double **parameter_iw, double **gv_iw)
{
   int i, j, k;
   double temp1, temp2;
   int state;
   HTS_SStream *sst;
   double *duration_mean, *duration_vari;
   double duration_remain;
   double rho = 0.0;

   /* initialize state sequence */
   sss->nstate = HTS_ModelSet_get_nstate(ms);
   sss->nstream = HTS_ModelSet_get_nstream(ms);
   sss->total_frame = 0;
   sss->total_state = HTS_Label_get_size(label) * sss->nstate;
   sss->duration = (int *) HTS_calloc(sss->total_state, sizeof(int));
   sss->sstream = (HTS_SStream *) HTS_calloc(sss->nstream, sizeof(HTS_SStream));
   for (i = 0; i < sss->nstream; i++) {
      sst = &sss->sstream[i];
      sst->vector_length = HTS_ModelSet_get_vector_length(ms, i);
      sst->mean = (double **) HTS_calloc(sss->total_state, sizeof(double *));
      sst->vari = (double **) HTS_calloc(sss->total_state, sizeof(double *));
      if (HTS_ModelSet_is_msd(ms, i))
         sst->msd = (double *) HTS_calloc(sss->total_state, sizeof(double));
      else
         sst->msd = NULL;
      for (j = 0; j < sss->total_state; j++) {
         sst->mean[j] =
             (double *) HTS_calloc(sst->vector_length, sizeof(double));
         sst->vari[j] =
             (double *) HTS_calloc(sst->vector_length, sizeof(double));
      }
   }

   /* check interpolation weights */
   for (i = 0, temp1 = 0.0;
        i < HTS_ModelSet_get_duration_interpolation_size(ms); i++)
      temp1 += duration_iw[i];
   for (i = 0; i < HTS_ModelSet_get_duration_interpolation_size(ms); i++)
      duration_iw[i] /= temp1;
   for (i = 0; i < sss->nstream; i++) {
      for (j = 0, temp1 = 0.0;
           j < HTS_ModelSet_get_parameter_interpolation_size(ms, i); j++)
         temp1 += parameter_iw[i][j];
      for (j = 0; j < HTS_ModelSet_get_parameter_interpolation_size(ms, i); j++)
         parameter_iw[i][j] /= temp1;
      if (HTS_ModelSet_use_gv(ms, i)) {
         for (j = 0, temp1 = 0.0;
              j < HTS_ModelSet_get_gv_interpolation_size(ms, i); j++)
            temp1 += gv_iw[i][j];
         for (j = 0; j < HTS_ModelSet_get_gv_interpolation_size(ms, i); j++)
            gv_iw[i][j] /= temp1;

      }
   }

   /* memory allocate for duration_mean & duration_vari */
   duration_mean = (double *) HTS_calloc(sss->nstate, sizeof(double));
   duration_mean -= 2;
   duration_vari = (double *) HTS_calloc(sss->nstate, sizeof(double));
   duration_vari -= 2;
   duration_remain = 0.0;

   /* determine rho */
   if (HTS_Label_get_speech_speed(label) != 1.0) {
      temp1 = 0.0;
      temp2 = 0.0;
      for (i = 0; i < HTS_Label_get_size(label); i++) {
         HTS_ModelSet_get_duration(ms, HTS_Label_get_string(label, i),
                                   duration_mean, duration_vari, duration_iw);
         for (j = 2; j <= sss->nstate + 1; j++) {
            temp1 += duration_mean[j];
            temp2 += duration_vari[j];
         }
      }
      rho = (temp1 / HTS_Label_get_speech_speed(label) - temp1) / temp2;
   }

   /* parse label file */
   for (i = 0, state = 0; i < HTS_Label_get_size(label); i++) {
      /* determine state duration */
      HTS_ModelSet_get_duration(ms, HTS_Label_get_string(label, i),
                                duration_mean, duration_vari, duration_iw);
      if (HTS_Label_get_frame_specified_flag(label, i)) {       /* use duration set by user */
         temp1 = 0.0;
         for (j = 2; j <= sss->nstate + 1; j++) {
            duration_mean[j] += rho * duration_vari[j];
            temp1 += duration_mean[j];
         }
         temp1 = HTS_Label_get_frame(label, i) / temp1;
         for (j = 2; j <= sss->nstate + 1; j++) {
            temp2 = temp1 * duration_mean[j];
            duration_mean[j] = (double) ((int) (temp2 + duration_remain + 0.5));
            if (duration_mean[j] < 1.0)
               duration_mean[j] = 1.0;
            duration_remain += temp2 - duration_mean[j];
         }
      } else {                  /* estimate using gauss */
         for (j = 2; j <= sss->nstate + 1; j++) {
            temp1 = duration_mean[j] + rho * duration_vari[j];
            duration_mean[j] = (double) ((int) (temp1 + duration_remain + 0.5));
            if (duration_mean[j] < 1.0)
               duration_mean[j] = 1.0;
            duration_remain += temp1 - duration_mean[j];
         }
      }

      /* determine parameter */
      for (j = 2; j <= sss->nstate + 1; j++) {
         sss->duration[state] = (int) duration_mean[j];
         for (k = 0; k < sss->nstream; k++) {
            sst = &sss->sstream[k];
            if (sst->msd)
               HTS_ModelSet_get_parameter(ms, HTS_Label_get_string(label, i),
                                          sst->mean[state], sst->vari[state],
                                          &sst->msd[state], k, j,
                                          parameter_iw[k]);
            else
               HTS_ModelSet_get_parameter(ms, HTS_Label_get_string(label, i),
                                          sst->mean[state], sst->vari[state],
                                          NULL, k, j, parameter_iw[k]);
         }
         sss->total_frame += sss->duration[state];
         state++;
      }
   }
   HTS_free(duration_mean + 2);
   HTS_free(duration_vari + 2);

   /* copy dynamic window */
   for (i = 0; i < sss->nstream; i++) {
      sst = &sss->sstream[i];
      sst->win_size = HTS_ModelSet_get_window_size(ms, i);
      sst->win_max_width = HTS_ModelSet_get_window_max_width(ms, i);
      sst->win_l_width = (int *) HTS_calloc(sst->win_size, sizeof(int));
      sst->win_r_width = (int *) HTS_calloc(sst->win_size, sizeof(int));
      sst->win_coefficient =
          (double **) HTS_calloc(sst->win_size, sizeof(double));
      for (j = 0; j < sst->win_size; j++) {
         sst->win_l_width[j] = HTS_ModelSet_get_window_left_width(ms, i, j);
         sst->win_r_width[j] = HTS_ModelSet_get_window_right_width(ms, i, j);
         if (sst->win_l_width[j] + sst->win_r_width[j] == 0)
            sst->win_coefficient[j] =
                (double *) HTS_calloc(-2 * sst->win_l_width[j] + 1,
                                      sizeof(double));
         else
            sst->win_coefficient[j] =
                (double *) HTS_calloc(-2 * sst->win_l_width[j], sizeof(double));
         sst->win_coefficient[j] -= sst->win_l_width[j];
         for (k = sst->win_l_width[j]; k <= sst->win_r_width[j]; k++)
            sst->win_coefficient[j][k] =
                HTS_ModelSet_get_window_coefficient(ms, i, j, k);
      }
   }

   /* determine GV */
   for (i = 0; i < sss->nstream; i++) {
      sst = &sss->sstream[i];
      if (HTS_ModelSet_use_gv(ms, i)) {
         sst->gv_mean =
             (double *) HTS_calloc(sst->vector_length / sst->win_size,
                                   sizeof(double));
         sst->gv_vari =
             (double *) HTS_calloc(sst->vector_length / sst->win_size,
                                   sizeof(double));
         HTS_ModelSet_get_gv(ms, sst->gv_mean, sst->gv_vari, i, gv_iw[i]);
      } else {
         sst->gv_mean = NULL;
         sst->gv_vari = NULL;
      }
   }
}

/* HTS_SStreamSet_get_nstream: get number of stream */
int HTS_SStreamSet_get_nstream(HTS_SStreamSet * sss)
{
   return sss->nstream;
}

/* HTS_SStreamSet_get_vector_length: get vector length */
int HTS_SStreamSet_get_vector_length(HTS_SStreamSet * sss, int stream_index)
{
   return sss->sstream[stream_index].vector_length;
}

/* HTS_SStreamSet_is_msd: get MSD flag */
HTS_Boolean HTS_SStreamSet_is_msd(HTS_SStreamSet * sss, int stream_index)
{
   return sss->sstream[stream_index].msd ? TRUE : FALSE;
}

/* HTS_SStreamSet_get_total_state: get total number of state */
int HTS_SStreamSet_get_total_state(HTS_SStreamSet * sss)
{
   return sss->total_state;
}

/* HTS_SStreamSet_get_total_frame: get total number of frame */
int HTS_SStreamSet_get_total_frame(HTS_SStreamSet * sss)
{
   return sss->total_frame;
}

/* HTS_SStreamSet_get_msd: get MSD parameter */
double HTS_SStreamSet_get_msd(HTS_SStreamSet * sss, int stream_index,
                              int state_index)
{
   return sss->sstream[stream_index].msd[state_index];
}

/* HTS_SStreamSet_window_size: get dynamic window size */
int HTS_SStreamSet_get_window_size(HTS_SStreamSet * sss, int stream_index)
{
   return sss->sstream[stream_index].win_size;
}

/* HTS_SStreamSet_get_window_left_width: get left width of dynamic window */
int HTS_SStreamSet_get_window_left_width(HTS_SStreamSet * sss,
                                         int stream_index, int window_index)
{
   return sss->sstream[stream_index].win_l_width[window_index];
}

/* HTS_SStreamSet_get_winodow_right_width: get right width of dynamic window */
int HTS_SStreamSet_get_window_right_width(HTS_SStreamSet * sss,
                                          int stream_index, int window_index)
{
   return sss->sstream[stream_index].win_r_width[window_index];
}

/* HTS_SStreamSet_get_window_coefficient: get coefficient of dynamic window */
double HTS_SStreamSet_get_window_coefficient(HTS_SStreamSet * sss,
                                             int stream_index,
                                             int window_index,
                                             int coefficient_index)
{
   return sss->sstream[stream_index].
       win_coefficient[window_index][coefficient_index];
}

/* HTS_SStreamSet_get_window_max_width: get max width of dynamic window */
int HTS_SStreamSet_get_window_max_width(HTS_SStreamSet * sss, int stream_index)
{
   return sss->sstream[stream_index].win_max_width;
}

/* HTS_SStreamSet_use_gv: get GV flag */
HTS_Boolean HTS_SStreamSet_use_gv(HTS_SStreamSet * sss, int stream_index)
{
   return sss->sstream[stream_index].gv_mean ? TRUE : FALSE;
}

/* HTS_SStreamSet_get_duration: get state duration */
int HTS_SStreamSet_get_duration(HTS_SStreamSet * sss, int state_index)
{
   return sss->duration[state_index];
}

/* HTS_SStreamSet_get_mean: get mean parameter */
double HTS_SStreamSet_get_mean(HTS_SStreamSet * sss,
                               int stream_index,
                               int state_index, int vector_index)
{
   return sss->sstream[stream_index].mean[state_index][vector_index];
}

/* HTS_SStreamSet_get_mean: set mean parameter */
void HTS_SStreamSet_set_mean(HTS_SStreamSet * sss, int stream_index,
                             int state_index, int vector_index, double f)
{
   sss->sstream[stream_index].mean[state_index][vector_index] = f;
}

/* HTS_SStreamSet_get_vari: get variance parameter */
double HTS_SStreamSet_get_vari(HTS_SStreamSet * sss,
                               int stream_index,
                               int state_index, int vector_index)
{
   return sss->sstream[stream_index].vari[state_index][vector_index];
}

/* HTS_SStreamSet_get_mean: set mean parameter */
void HTS_SStreamSet_set_vari(HTS_SStreamSet * sss, int stream_index,
                             int state_index, int vector_index, double f)
{
   sss->sstream[stream_index].vari[state_index][vector_index] = f;
}

/* HTS_SStreamSet_get_gv_mean: get GV mean parameter */
double HTS_SStreamSet_get_gv_mean(HTS_SStreamSet * sss,
                                  int stream_index, int vector_index)
{
   return sss->sstream[stream_index].gv_mean[vector_index];
}

/* HTS_SStreamSet_get_gv_mean: get GV variance parameter */
double HTS_SStreamSet_get_gv_vari(HTS_SStreamSet * sss,
                                  int stream_index, int vector_index)
{
   return sss->sstream[stream_index].gv_vari[vector_index];
}

/* HTS_SStreamSet_clear: free state stream set */
void HTS_SStreamSet_clear(HTS_SStreamSet * sss)
{
   int i, j;
   HTS_SStream *sst;

   if (sss->sstream) {
      for (i = 0; i < sss->nstream; i++) {
         sst = &sss->sstream[i];
         for (j = 0; j < sss->total_state; j++) {
            HTS_free(sst->mean[j]);
            HTS_free(sst->vari[j]);
         }
         if (sst->msd)
            HTS_free(sst->msd);
         HTS_free(sst->mean);
         HTS_free(sst->vari);
         for (j = sst->win_size - 1; j >= 0; j--) {
            sst->win_coefficient[j] += sst->win_l_width[j];
            HTS_free(sst->win_coefficient[j]);
         }
         HTS_free(sst->win_coefficient);
         HTS_free(sst->win_l_width);
         HTS_free(sst->win_r_width);
         if (sst->gv_mean)
            HTS_free(sst->gv_mean);
         if (sst->gv_vari)
            HTS_free(sst->gv_vari);
      }
      HTS_free(sss->sstream);
   }
   if (sss->duration)
      HTS_free(sss->duration);

   HTS_SStreamSet_initialize(sss);
}
