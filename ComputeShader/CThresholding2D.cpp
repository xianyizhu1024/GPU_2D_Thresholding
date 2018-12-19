#include "CThresholding2D.h"
#include "GLFW/glfw3.h"
namespace cs
{
	void CThresholding2D::set_image_bins2d()
	{
		
		glGenTextures(1, &m_tex_bins2d);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_tex_bins2d);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, m_grayscale, m_grayscale);

		//glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, 1024, 1, 0, GL_RED, GL_UNSIGNED_INT, dat);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 256, 1, 0, GL_RGBA, GL_FLOAT, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);

		m_image_bins2d = 3;


	}
	void CThresholding2D::setup1(std::string pic_path,bool* pkey,int grayscale, int nearbyorder)
	{
		m_grayscale = grayscale;
		m_nearbyorder = nearbyorder;
		set_gpfm_pic(pic_path);
		set_shader_hist1d("computeshader/csh_hist1d.comp");
		set_shader_smooth("computeshader/csh_smooth.comp");
		set_shader_binary("computeshader/csh_binary.comp");
		set_shader_initsat("computeshader/csh_initSAT.comp");
		set_shader_sat("computeshader/csh_SAT.comp");
		set_shader_criteria("computeshader/csh_criteria.comp");
		set_shader_most("computeshader/csh_reduce.comp");
			set_shader_smooth_sf("computeshader/csh_smooth_sf.comp");
			set_shader_most_stride("computeshader/csh_reduce_stride.comp");
			set_shader_hist2d("computeshader/csh_hist2d_v2.comp");
			set_shader_sat_block("computeshader/csh_SAT_block_s11.comp", 
								 "computeshader/csh_SAT_block_s12.comp", 
								 "computeshader/csh_SAT_block_s23.comp");
			set_shader_mostsh("computeshader/csh_reduce_s01.comp");

		set_image_pic();
		set_image_bins();
		set_image_bins2d();
		set_image_sat_criteria();
		m_pkey = pkey;

		g_pbins2d = new GLuint[m_grayscale*m_grayscale];
	}
	void CThresholding2D::set_image_sat_criteria()
	{
		// sat
		glGenTextures(1, &m_tex_SATa);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_tex_SATa);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, m_grayscale, m_grayscale);
		m_image_SATa = 0;

		// sat intermiddle
		glGenTextures(1, &m_tex_SATc);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_tex_SATc);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, m_grayscale, m_grayscale);
		m_image_SATc = 1;

		// sat result
		glGenTextures(1, &m_tex_SATb);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_tex_SATb);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, m_grayscale, m_grayscale);
		m_image_SATb = 2;
		

		glGenTextures(1, &m_tex_criteria);
		glBindTexture(GL_TEXTURE_2D, m_tex_criteria);
		glTexStorage2D(GL_TEXTURE_2D, int(log2(m_grayscale + 1))+1, GL_RGBA32F, m_grayscale, m_grayscale);
		m_image_criteria = 0;

		glGenTextures(1, &m_tex_most);
		glBindTexture(GL_TEXTURE_2D, m_tex_most);
		glTexStorage2D(GL_TEXTURE_2D, int(log2(m_grayscale + 1))+1, GL_RGBA32F, m_grayscale, m_grayscale);
		m_image_most = 1;

		glGenTextures(1, &m_tex_most_tmp);
		glBindTexture(GL_TEXTURE_2D, m_tex_most_tmp);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, m_grayscale, m_grayscale);
		m_image_most_tmp = 2;

		glBindTexture(GL_TEXTURE_2D, 0);

	}
	void CThresholding2D::set_gpfm_pic(std::string path)
	{
		m_path_pic = path;
		m_pic.load(path.c_str());
	//	m_tmp.load(path.c_str());
		m_width_pic = m_pic.w;
		m_height_pic = m_pic.h;
	
		set_wrkgrp();
	}

	void CThresholding2D::set_wrkgrp()
	{
		m_wrk_grp_x = (m_width_pic + WRK_grp_size_x) / WRK_grp_size_x;
		m_wrk_grp_y = (m_height_pic + WRK_grp_size_y) / WRK_grp_size_y;
		m_wrk_grp_z = 1; 

	}
	void CThresholding2D::set_image_pic()
	{
		
		 m_tmp.load(m_width_pic,m_height_pic,m_pic.fm);
		 /* for (int i = 0; i < m_width_pic; i++)
			  for (int j = 0; j < m_height_pic; j++)
			  tmp.lookup_nearest();*/
		 ///  均值
		
		 //int i, j;
		 //for (i = 0; i < m_height_pic; i++)
		 //{

			// for (j = 0; j < m_width_pic; j++)
			// {
			//	 int p, q, t, k;
			//	 float sum;
			//	 sum = 0.0f;
			//	 for (q = -1; q < 2; q++)
			//	 {
			//		 for (p = -1; p < 2; p++)
			//		 {
			//			 t = G_CLAMP(q + i, 0, m_height_pic - 1);
			//			 k = G_CLAMP(p + j, 0, m_width_pic - 1);
			//			 sum = sum + m_pic.lookup_nearest(k, t).x;
			//		 }
			//	 }
			//	 m_tmp.pm[i][j].x = m_pic.lookup_nearest(j, i).x;
			//	 m_tmp.pm[i][j].y = sum / 9.0;
			//	 m_tmp.pm[i][j].w = 0.f;//round(sum / 9.0 * 255);
			//	 m_tmp.pm[i][j].z = 0.f;//round(m_pic.lookup_nearest(j, i).x * 255);

			// }

		 //}
		 //m_tmp.save("comp_tmp.pfm");
		glGenTextures(1, &m_tex_pic);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_tex_pic);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, m_width_pic, m_height_pic);
		//  gltexsubImage2D 和
		glTexSubImage2D(GL_TEXTURE_2D, 0,0,0, m_width_pic, m_height_pic,  GL_RGBA, GL_FLOAT, m_tmp.fm);
		glBindTexture(GL_TEXTURE_2D, 0);

		m_image_pic = 0;
		

		glGenTextures(1, &m_tex_smooth_intensity_ave);
		glBindTexture(GL_TEXTURE_2D, m_tex_smooth_intensity_ave);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, m_width_pic, m_height_pic);
		
		glGenTextures(1, &m_tex_smooth_tmp);
		glBindTexture(GL_TEXTURE_2D, m_tex_smooth_tmp);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, m_width_pic, m_height_pic);

		glGenTextures(1, &m_tex_binary);
		glBindTexture(GL_TEXTURE_2D, m_tex_binary);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, m_width_pic, m_height_pic);
	}



	void CThresholding2D::set_image_bins()
	{
		static FLOAT3 dat[1024] = { 0 };
		glGenTextures(1, &m_tex_bins);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_tex_bins);
		glTexStorage2D(GL_TEXTURE_2D,1,GL_R32UI,1024,1);

		//glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, 1024, 1, 0, GL_RED, GL_UNSIGNED_INT, dat);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 256, 1, 0, GL_RGBA, GL_FLOAT, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);

		m_image_bins = 1;
		

	}
	void CThresholding2D::smooth(int r)
	{
		m_psh_smooth->begin();
		glBindImageTexture(m_image_pic, m_tex_pic, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		glBindImageTexture(1		  , m_tex_smooth_intensity_ave, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

		glUniform1i(glGetUniformLocation(m_psh_smooth->Program, "width_pic"), m_width_pic);
		glUniform1i(glGetUniformLocation(m_psh_smooth->Program, "height_pic"), m_height_pic);
		glUniform1i(glGetUniformLocation(m_psh_smooth->Program, "nearbyorder"), r);
		//glDispatchComputeGroupSizeARB(
		//	m_wrk_grp_x, m_wrk_grp_y, m_wrk_grp_z,
		//	WRK_grp_size_x, WRK_grp_size_y , WRK_grp_size_z );

		glDispatchCompute(m_wrk_grp_x, m_wrk_grp_y, m_wrk_grp_z);
		m_psh_smooth->end();
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		if (m_pkey[GLFW_KEY_S])
		{
			GPf4 output;
			output.load(m_width_pic,m_height_pic);
			
			glBindTexture(GL_TEXTURE_2D, m_tex_smooth_intensity_ave);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, output.fm);
			output.save("comp_smooth.pfm");
		}
	}

	void CThresholding2D::smooth_sf(int order )
	{
		int wrk_grp_y;
		// 每行
		m_psh_smooth_sf->begin();
		glBindImageTexture(m_image_pic, m_tex_pic, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		glBindImageTexture(1, m_tex_smooth_tmp, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
			glUniform1ui(glGetUniformLocation(m_psh_smooth_sf->Program, "isRow"), true);
			glUniform1i(glGetUniformLocation(m_psh_smooth_sf->Program, "width_pic"), m_width_pic);
			glUniform1i(glGetUniformLocation(m_psh_smooth_sf->Program, "height_pic"), m_height_pic);
			glUniform1i(glGetUniformLocation(m_psh_smooth_sf->Program, "nearbyorder"), order);
				wrk_grp_y = (m_width_pic + 400) / 400;
				glDispatchCompute(m_height_pic, wrk_grp_y, 1);
		m_psh_smooth_sf->end();
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		//每列
		m_psh_smooth_sf->begin();
		glBindImageTexture(0, m_tex_smooth_tmp, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		glBindImageTexture(1, m_tex_smooth_intensity_ave, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
			glUniform1ui(glGetUniformLocation(m_psh_smooth_sf->Program, "isRow"), false);
			glUniform1i(glGetUniformLocation(m_psh_smooth_sf->Program, "width_pic"), m_width_pic);
			glUniform1i(glGetUniformLocation(m_psh_smooth_sf->Program, "height_pic"), m_height_pic);
			glUniform1i(glGetUniformLocation(m_psh_smooth_sf->Program, "nearbyorder"), order);
				wrk_grp_y = (m_height_pic + 400) / 400;
				glDispatchCompute(m_width_pic, wrk_grp_y, 1);
		m_psh_smooth_sf->end();
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		
		if (m_pkey[GLFW_KEY_S])
		{
			GPf4 output;
			output.load(m_width_pic, m_height_pic);

			glBindTexture(GL_TEXTURE_2D, m_tex_smooth_intensity_ave);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, output.fm);
			output.save("comp_smooth_sf.pfm");

			glBindTexture(GL_TEXTURE_2D, m_tex_smooth_tmp);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, output.fm);
			output.save("comp_smooth_sf_tmp.pfm");
		}

	}


	
	void CThresholding2D::genhist(bool is1d)
	{
		
	//	static GLuint dat[1024] = { 0 };
		static GLuint dat_2d[1024][1024] = { 0 };
		
	//	glBindTexture(GL_TEXTURE_2D, m_tex_bins);
	//	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1024, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, dat);

		glBindTexture(GL_TEXTURE_2D, m_tex_bins2d);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_grayscale, m_grayscale, GL_RED_INTEGER, GL_UNSIGNED_INT, dat_2d);
		glBindTexture(GL_TEXTURE_2D, 0);
		
		m_psh_hist1d->begin();
		glBindImageTexture(m_image_pic, m_tex_smooth_intensity_ave, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		glBindImageTexture(m_image_bins, m_tex_bins, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
	//	glBindImageTexture(2, m_tex_smooth, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		glBindImageTexture(m_image_bins2d, m_tex_bins2d, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

		glUniform1ui(glGetUniformLocation(m_psh_hist1d->Program, "is1d"), is1d);
		glUniform1i(glGetUniformLocation(m_psh_hist1d->Program,"width_pic") , m_width_pic);
		glUniform1i(glGetUniformLocation(m_psh_hist1d->Program, "height_pic"), m_height_pic);
		glUniform1i(glGetUniformLocation(m_psh_hist1d->Program, "grayscale"), m_grayscale);
		//glDispatchComputeGroupSizeARB(
		//	m_wrk_grp_x, m_wrk_grp_y, m_wrk_grp_z,
		//	WRK_grp_size_x, WRK_grp_size_y , WRK_grp_size_z );
		
		glDispatchCompute(m_wrk_grp_x, m_wrk_grp_y, m_wrk_grp_z);
		m_psh_hist1d->end();
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		
		if (m_pkey[GLFW_KEY_S])
		{
			
		//	static GLuint tmp[1024] = { 0 };
		//	glBindTexture(GL_TEXTURE_2D, m_tex_bins);
		//	glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, tmp);

			static GLuint * pbin2d=NULL;
			if (pbin2d == NULL)
				pbin2d = new GLuint[m_grayscale*m_grayscale];
			glBindTexture(GL_TEXTURE_2D, m_tex_bins2d);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, pbin2d);

			/*static GLfloat tmp_2dfloat[256 * 256];
			for (size_t i = 0; i < 256 * 256; i++)
			{
				tmp_2dfloat[i] = pbin2d[i];
			}*/
			//float *p = (float *)pbin2d;
			GPfm output; 
			output.load(m_grayscale, m_grayscale, pbin2d, NULL, NULL);
			output.save("comp_hist2d.pfm");
		}
			/*tmp.save("comp_hist1d.pfm");*/
		//glBindTexture(GL_TEXTURE_2D, m_tex_bins);
		//glGetTexImage(GL_TEXTURE_2D, 0, GL_R32UI, GL_UNSIGNED_INT, tmp);

		/*GPf4  tmp;
		tmp.load(256, 1);*/
		

		/*tmp.save("comp_hist1d.pfm");*/

		//memset(dat, 0, sizeof(dat));
	}

	void CThresholding2D::genhist_v2()
	{
		// for testing when coding
		static int flag = 0;
		static GLfloat *pDat ;
		if (flag){
			pDat = new GLfloat[m_width_pic*m_height_pic * 4];
			for (int i = 0; i < m_width_pic * m_height_pic * 4; i += 1) { pDat[i] = 1.0 / 255; }
			flag = 0;
			glBindTexture(GL_TEXTURE_2D, m_tex_smooth_intensity_ave);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width_pic, m_height_pic, GL_RGBA, GL_FLOAT, pDat);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		

		m_psh_histsh->begin();
			glBindImageTexture(m_image_pic, m_tex_smooth_intensity_ave, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
			glBindImageTexture(m_image_bins, m_tex_bins, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
			//	glBindImageTexture(2, m_tex_smooth, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
			glBindImageTexture(m_image_bins2d, m_tex_bins2d, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
			glUniform1i(glGetUniformLocation(m_psh_histsh->Program, "width_pic"), m_width_pic);
			glUniform1i(glGetUniformLocation(m_psh_histsh->Program, "height_pic"), m_height_pic);
			glUniform1i(glGetUniformLocation(m_psh_histsh->Program, "grayscale"), m_grayscale);
			glDispatchCompute(8, 8, m_wrk_grp_z);
		m_psh_histsh->end();
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		if (m_pkey[GLFW_KEY_S] )
		{
			// glGetTexImage
			static GLuint * pbin2d = NULL;
			if (pbin2d == NULL)
				pbin2d = new GLuint[m_grayscale*m_grayscale];
			glBindTexture(GL_TEXTURE_2D, m_tex_bins2d);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, pbin2d);

			// GPfm <- 
			GPfm output;
			output.load(m_grayscale, m_grayscale, pbin2d, NULL, NULL);
			output.save("comp_hist2d_v2.pfm");
		}
		
	}

	#define MAX_GRAYSCALE 1024
	float g_phist2d_norm[MAX_GRAYSCALE][MAX_GRAYSCALE];
	float g_P0[MAX_GRAYSCALE][MAX_GRAYSCALE];
	float g_Ui[MAX_GRAYSCALE][MAX_GRAYSCALE];
	float g_Uj[MAX_GRAYSCALE][MAX_GRAYSCALE];
	float g_H0[MAX_GRAYSCALE][MAX_GRAYSCALE];
	int g_s ;
	int g_t ;
	void CThresholding2D::cal_criteriaCPU(bool isOTSU)
	{
		// copy 2D histogram from device memory to host memory
		glBindTexture(GL_TEXTURE_2D, m_tex_bins2d);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, g_pbins2d);
		glBindTexture(GL_TEXTURE_2D, 0);
		
		// normalize hist2D
		int i, j;
		for (i = 0; i < m_grayscale; i++)
			for (j = 0; j < m_grayscale; j++)
			{
				g_H0[i][j] = .0f;
				g_phist2d_norm[i][j] = 1.0f*g_pbins2d[j*m_grayscale + i] / m_height_pic / m_width_pic;
			}

		// compute P0 Ui Uj  H0
		float ttt;
		// P0 
		g_P0[0][0] = g_phist2d_norm[0][0];

		for (i = 1; i < m_grayscale; i++)
		{
			g_P0[i][0] = g_P0[i - 1][0] + g_phist2d_norm[i][0];
			g_P0[0][i] = g_P0[0][i - 1] + g_phist2d_norm[0][i];
		}

		for (i = 1; i < m_grayscale; i++)	{
			for (j = 1; j < m_grayscale; j++)	{
				g_P0[i][j] = g_P0[i - 1][j] + g_P0[i][j - 1] - g_P0[i - 1][j - 1] + g_phist2d_norm[i][j];
			}
		}

		if (isOTSU){ //OTSU
			// Ui Uj
			g_Ui[0][0] = 0;
			g_Uj[0][0] = 0;

			for (i = 1; i < m_grayscale; i++)
			{
				g_Ui[i][0] = g_Ui[i - 1][0] + i*g_phist2d_norm[i][0];
				g_Ui[0][i] = g_Ui[0][i - 1];

				g_Uj[0][i] = g_Uj[0][i - 1] + i*g_phist2d_norm[0][i];
				g_Uj[i][0] = g_Uj[i - 1][0];

			}

			for (i = 1; i < m_grayscale; i++)
			{
				for (j = 1; j < m_grayscale; j++)
				{
					g_Ui[i][j] = g_Ui[i - 1][j] + g_Ui[i][j - 1] - g_Ui[i - 1][j - 1] + i*g_phist2d_norm[i][j];
					g_Uj[i][j] = g_Uj[i - 1][j] + g_Uj[i][j - 1] - g_Uj[i - 1][j - 1] + j*g_phist2d_norm[i][j];
				}
			}
		}
		else{ // entropy
			// H0
			g_H0[0][0] = .0f;
			if ( g_phist2d_norm[0][0]>.0f)
	 			g_H0[0][0] = -g_phist2d_norm[0][0]*log2(g_phist2d_norm[0][0]);

			for (i = 1; i < m_grayscale; i++){
	 			ttt = .0f;
				if ( g_phist2d_norm[i][0]>.0f)
	 				ttt= - g_phist2d_norm[i][0] * log2(g_phist2d_norm[i][0]);
	 			g_H0[i][0] = g_H0[i - 1][0] + ttt;

	 			ttt = .0f;
				if ( g_phist2d_norm[0][i] > .0f)
	 				ttt = -g_phist2d_norm[0][i] * log2(g_phist2d_norm[0][i]);
	 			g_H0[0][i] = g_H0[0][i - 1] + ttt;
			}

			for (i = 1; i < m_grayscale; i++)	{
				for (j = 1; j < m_grayscale; j++)	{
					ttt = .0f;
					if (g_phist2d_norm[i][j] > .0f)
					 	ttt = -g_phist2d_norm[i][j] * log2(g_phist2d_norm[i][j]);
					g_H0[i][j] = g_H0[i - 1][j] + g_H0[i][j - 1] - g_H0[i - 1][j - 1] +ttt;
				}
			}
		}		

		

		// cal criteria (variance or entropy)
		float uti, utj, temp, delta,phi,temp2,w0;
		uti = g_Ui[m_grayscale - 1][m_grayscale - 1];
		utj = g_Uj[m_grayscale - 1][m_grayscale - 1];
		temp = .0f;
		if (isOTSU){
			for (i = 0; i < m_grayscale; i++){
				for (j = 0; j < m_grayscale; j++){
					w0 = g_P0[i][j];
					if (w0 > .0f && w0 < 1.0f){
						delta =
							(
							(w0 * uti - g_Ui[i][j])*(w0 * uti - g_Ui[i][j])
							+
							(w0 * utj - g_Uj[i][j])*(w0 * utj - g_Uj[i][j])
							)
							/
							(w0*(1 - w0));
					}
					else{
						delta = .0f;
					}
					if (delta > temp){
						temp = delta;
						g_s = i;
						g_t = j;
						/*printf("s %d t %d delta %f\n", s, t, temp);*/
					}
				}
			}
		}

		temp2 = -10000;
		if (isOTSU == false){
			for (i = 0; i < m_grayscale; i++)	{
				for (j = 0; j < m_grayscale; j++)	{
					w0 = g_P0[i][j];
					phi = 0;
					
					if (w0 > .0f && w0 < 1.0f)	{
						phi = log2((1 - w0)*w0) + g_H0[i][j] / w0 + (g_H0[m_grayscale - 1][m_grayscale - 1] - g_H0[i][j]) / (1 - w0);
					}

					if (phi > temp2)	{
						temp2 = phi;
						g_s = i;
						g_t = j;						
					}
				}
			}
		}
		
		// copy threshold vector from host memory to device memory
		static float oste[4] = { 0 };
		oste[0] = temp;
		oste[1] = g_s;
		oste[2] = g_t;
		//printf("%d %d\n", g_s, g_t);
		oste[3] = temp2;
		int steps = int(log2(m_grayscale + 1));
		GLuint texid_cst = (steps % 2 == 0 ? m_tex_criteria : m_tex_most);
		glBindTexture(GL_TEXTURE_2D, m_tex_most);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1, 1, GL_RGBA, GL_FLOAT, oste);
		glBindTexture(GL_TEXTURE_2D, 0);

		if (m_pkey[GLFW_KEY_S] )
		{
			GPf4 output;
			output.load(m_grayscale, m_grayscale);
			glBindTexture(GL_TEXTURE_2D, m_tex_most);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, output.fm);
			glBindTexture(GL_TEXTURE_2D, 0);
			output.save("comp_mostCPU.pfm");
		}
	}

	void CThresholding2D::init_sat(int isOTSU )
	{
		m_psh_initsat->begin();
		glBindImageTexture(m_image_SATa, m_tex_SATa,0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		glBindImageTexture(m_image_bins2d, m_tex_bins2d, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
		glUniform1i(glGetUniformLocation(m_psh_initsat->Program, "num_pixel"), m_width_pic *m_height_pic);
		glUniform1i(glGetUniformLocation(m_psh_initsat->Program, "isOTSU"), isOTSU);

		glDispatchCompute(m_grayscale / 16, m_grayscale / 16, 1);
		m_psh_initsat->end();
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		if (m_pkey[GLFW_KEY_S] )	{
			GPf4 output;
			output.load(m_grayscale, m_grayscale);
			glBindTexture(GL_TEXTURE_2D, m_tex_SATa);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, output.fm);
			output.save("comp_init_sat.pfm");

			
			static GLuint * pbin2d = NULL;
			if (pbin2d == NULL)
				pbin2d = new GLuint[m_grayscale*m_grayscale];
			glBindTexture(GL_TEXTURE_2D, m_tex_bins2d);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, pbin2d);
			glBindTexture(GL_TEXTURE_2D, 0);
			
			GPfm output2;
			output2.load(m_grayscale, m_grayscale, pbin2d, NULL, NULL);
			output2.save("comp_init_sat_hist2d.pfm");
		}
	}

	void CThresholding2D::sat()
	{

		m_psh_sat->begin();
		glBindImageTexture(m_image_SATa, m_tex_SATa, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		glBindImageTexture(m_image_SATc, m_tex_SATc, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glBindImageTexture(m_image_SATb, m_tex_SATb, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	//	glUniform1i(glGetUniformLocation(m_psh_sat->Program, "num_pixel"), m_width_pic *m_height_pic);
		glUniform1i(glGetUniformLocation(m_psh_sat->Program, "graylevel"), m_grayscale);
		glDispatchCompute(m_grayscale/32, 1, 1);
		m_psh_sat->end();
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		if (m_pkey[GLFW_KEY_S])	{
			GPf4 output;
			output.load(m_grayscale, m_grayscale);
			glBindTexture(GL_TEXTURE_2D, m_tex_SATb);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, output.fm);
			output.save("comp_sat.pfm");
		}
	}

	void CThresholding2D::sat_block()
	{
		// all bins = 1, for testing when coding
		/*static int flag = 1;
		static GLfloat dat_2d[256*256*4] ;
		if (flag){
			for (int i = 0; i < 256 * 256 * 4; i++) { dat_2d[i] = 1.0f; }
			flag = 0;
		}
		glBindTexture(GL_TEXTURE_2D, m_tex_SATa);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_grayscale, m_grayscale, GL_RGBA, GL_FLOAT, dat_2d);
		glBindTexture(GL_TEXTURE_2D, 0);*/

		m_psh_satblock01->begin();
		glBindImageTexture(m_image_SATa, m_tex_SATa, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		glBindImageTexture(m_image_SATc, m_tex_SATc, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glBindImageTexture(m_image_SATb, m_tex_SATb, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		//	glUniform1i(glGetUniformLocation(m_psh_sat->Program, "num_pixel"), m_width_pic *m_height_pic);
		glUniform1i(glGetUniformLocation(m_psh_satblock01->Program, "graylevel"), m_grayscale);
		glDispatchCompute(m_grayscale / 32 * m_grayscale / 32, 1, 1);
		m_psh_satblock01->end();
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		
		if (m_pkey[GLFW_KEY_S] )
		{
			GPf4 output;
			output.load(m_grayscale, m_grayscale);
			glBindTexture(GL_TEXTURE_2D, m_tex_SATc);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, output.fm);
			output.save("comp_sat_block01_c.pfm");

			output.load(m_grayscale, m_grayscale);
			glBindTexture(GL_TEXTURE_2D, m_tex_SATa);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, output.fm);
			output.save("comp_sat_block01_a.pfm");
		}

		m_psh_satblock02->begin();
		glBindImageTexture(m_image_SATa, m_tex_SATa, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glBindImageTexture(m_image_SATc, m_tex_SATc, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glBindImageTexture(m_image_SATb, m_tex_SATb, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		//	glUniform1i(glGetUniformLocation(m_psh_sat->Program, "num_pixel"), m_width_pic *m_height_pic);
		glUniform1i(glGetUniformLocation(m_psh_satblock02->Program, "graylevel"), m_grayscale);
		glDispatchCompute(m_grayscale / 32 , 1, 1);
		m_psh_satblock02->end();
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		if (m_pkey[GLFW_KEY_S] )
		{
			GPf4 output;
			output.load(m_grayscale, m_grayscale);
			glBindTexture(GL_TEXTURE_2D, m_tex_SATc);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, output.fm);
			output.save("comp_sat_block02_c.pfm");

			
		}

		m_psh_satblock03->begin();
		glBindImageTexture(m_image_SATa, m_tex_SATa, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		glBindImageTexture(m_image_SATc, m_tex_SATc, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glBindImageTexture(m_image_SATb, m_tex_SATb, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		//	glUniform1i(glGetUniformLocation(m_psh_sat->Program, "num_pixel"), m_width_pic *m_height_pic);
		glUniform1i(glGetUniformLocation(m_psh_satblock03->Program, "graylevel"), m_grayscale);
		glDispatchCompute(m_grayscale / 32 * m_grayscale / 32, 1, 1);
		m_psh_satblock03->end();
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		if (m_pkey[GLFW_KEY_S] )
		{
			GPf4 output;
			output.load(m_grayscale, m_grayscale);
			glBindTexture(GL_TEXTURE_2D, m_tex_SATb);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, output.fm);
			output.save("comp_sat_block03_b.pfm");
		}

	}
	
	void CThresholding2D::cal_criteria(int isOTSU)
	{
		m_psh_criteria->begin();
			glBindImageTexture(m_image_criteria, m_tex_criteria, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
			glBindImageTexture(m_image_SATb, m_tex_SATb, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
			glUniform1i(glGetUniformLocation(m_psh_criteria->Program, "graylevel"), m_grayscale);
			glUniform1i(glGetUniformLocation(m_psh_criteria->Program, "isOTSU"), isOTSU);

			glDispatchCompute(m_grayscale/16, m_grayscale / 16, 1);
		m_psh_criteria->end();
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		if (m_pkey[GLFW_KEY_S] )
		{
			GPf4 output;
			output.load(m_grayscale, m_grayscale);
			glBindTexture(GL_TEXTURE_2D, m_tex_criteria);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, output.fm);
			output.save("comp_criteria.pfm");
		}
	}

	void CThresholding2D::select_most()
	{
		// 奇数 m_tex_most ; 偶数 m_tex_criteria
		GLuint wtex = m_tex_most;
		GLuint rtex = m_tex_criteria;
		int i, threads1 = m_grayscale;
		int steps = int(log2(m_grayscale + 1));
		for (i = 0; i < steps; i++)
		{
			threads1 /= 2;
			m_psh_most->begin();
				glBindImageTexture(m_image_most, wtex, i + 1, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
				glBindImageTexture(m_image_criteria, rtex, i, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
				glUniform1i(glGetUniformLocation(m_psh_most->Program, "threads1"), threads1);
				glDispatchCompute(512 / 16, 512 / 16, 1);
			m_psh_most->end();
			g_swap(wtex, rtex);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		}
		if (m_pkey[GLFW_KEY_S])
		{
			GPf4 output;
			output.load(1, 1);
			glBindTexture(GL_TEXTURE_2D, rtex);
			glGetTexImage(GL_TEXTURE_2D, steps, GL_RGBA, GL_FLOAT, output.fm);
			output.save("comp_most.pfm");
		}

	}

	void CThresholding2D::select_most_stride(int stride)  //stride = 2 , 4 , 8 ...
	{
		// 奇数 m_tex_most ; 偶数 m_tex_criteria
		GLuint wtex = m_tex_most;
		GLuint rtex = m_tex_criteria;
		int i, threads1 = m_grayscale;

		int steps = int(log(m_grayscale + 1) / log(stride));
		for (i = 0; i < steps; i++)
		{
			threads1 /= stride;
			m_psh_most_stride->begin();
				glBindImageTexture(m_image_most	   , wtex, (i +1)*int(log2(stride)), GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
				glBindImageTexture(m_image_criteria, rtex, i	 *int(log2(stride)), GL_FALSE, 0,  GL_READ_ONLY, GL_RGBA32F);
				glUniform1i(glGetUniformLocation(m_psh_most_stride->Program, "threads1"), threads1);
				glUniform1i(glGetUniformLocation(m_psh_most_stride->Program, "stride"), stride);
				glDispatchCompute(threads1 / 16, threads1 / 16, 1);
			m_psh_most_stride->end();
			g_swap(wtex, rtex);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		}
		m_tex_most = wtex ;
		m_tex_criteria = rtex ;

		if (m_pkey[GLFW_KEY_S])
		{
			GPf4 output;
			output.load(1, 1);
			glBindTexture(GL_TEXTURE_2D, rtex);
			glGetTexImage(GL_TEXTURE_2D, steps*int(log2(stride)), GL_RGBA, GL_FLOAT, output.fm);
			output.save("comp_most_stride.pfm");
		}

	}

	void CThresholding2D::select_most_shared(int stride)  
	{
		////  for testing when coding
		/*static int flag = 1;
		static GLfloat dat_2d[256*256*4] ;
		if (flag){
			for (int i = 0; i < 256 * 256*4 ; i++) { dat_2d[i] = i; }
			flag = 0;
		}
		glBindTexture(GL_TEXTURE_2D, m_tex_criteria);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_grayscale, m_grayscale, GL_RGBA, GL_FLOAT, dat_2d);
		glBindTexture(GL_TEXTURE_2D, 0);*/

		m_psh_most_shared01->begin();
			glBindImageTexture(m_image_most_tmp, m_tex_most_tmp, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
			glBindImageTexture(m_image_criteria, m_tex_criteria, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
			glUniform1i(glGetUniformLocation(m_psh_most_shared01->Program, "graylevel"), m_grayscale);
			glUniform1i(glGetUniformLocation(m_psh_most_shared01->Program, "stride"), stride);
			glUniform1i(glGetUniformLocation(m_psh_most_shared01->Program, "ncell"), 1024);
			glUniform1i(glGetUniformLocation(m_psh_most_shared01->Program, "isTmp"), 1);
			glDispatchCompute(m_grayscale * m_grayscale / 1024, 1, 1);
		m_psh_most_shared01->end();
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		
		m_psh_most_shared01->begin();
			glBindImageTexture(m_image_most, m_tex_most, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
			glBindImageTexture(m_image_most_tmp, m_tex_most_tmp, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
			glUniform1i(glGetUniformLocation(m_psh_most_shared01->Program, "graylevel"), m_grayscale);
			glUniform1i(glGetUniformLocation(m_psh_most_shared01->Program, "stride"), stride);
			glUniform1i(glGetUniformLocation(m_psh_most_shared01->Program, "ncell"), m_grayscale * m_grayscale / 1024);
			glUniform1i(glGetUniformLocation(m_psh_most_shared01->Program, "isTmp"), 0);
			glDispatchCompute(1, 1, 1);
		m_psh_most_shared01->end();
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		if (m_pkey[GLFW_KEY_S] )
		{
			GPf4 output;
			output.load(m_grayscale, m_grayscale);
			glBindTexture(GL_TEXTURE_2D, m_tex_most_tmp);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, output.fm);
			glBindTexture(GL_TEXTURE_2D, 0);
			output.save("comp_most_shared_tmp.pfm");
			
			glBindTexture(GL_TEXTURE_2D, m_tex_most);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, output.fm);
			glBindTexture(GL_TEXTURE_2D, 0);
			output.save("comp_most_shared.pfm");
		}

	}

	void CThresholding2D::binary(GLuint texid_cst)
	{
		int steps   = int(log2(m_grayscale + 1));
		if (texid_cst == -10)
		{
			steps = int(log2(m_grayscale + 1));
			texid_cst = (steps % 2 == 0 ? m_tex_criteria : m_tex_most);
		}
		
		steps = int(log2(m_grayscale + 1));
		texid_cst = m_tex_criteria;

		m_psh_binary->begin();
			glBindImageTexture(0, m_tex_smooth_intensity_ave, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
			glBindImageTexture(1, m_tex_binary, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
			glBindImageTexture(2, m_tex_most, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
			glUniform1i(glGetUniformLocation(m_psh_binary->Program, "width_pic"), m_width_pic);
			glUniform1i(glGetUniformLocation(m_psh_binary->Program, "height_pic"), m_height_pic);
			glUniform1i(glGetUniformLocation(m_psh_binary->Program, "graylevel"), m_grayscale);
			glDispatchCompute(m_wrk_grp_x, m_wrk_grp_y, m_wrk_grp_z);
		m_psh_binary->end();
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		if (m_pkey[GLFW_KEY_S]  )
		{
			GPf4 output;
			output.load(m_width_pic, m_height_pic);

			glBindTexture(GL_TEXTURE_2D, m_tex_binary);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, output.fm);
			output.save("comp_binary.pfm");

		}

	}
}