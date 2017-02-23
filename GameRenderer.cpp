/// \file gamerenderer.cpp
/// \brief Direct3D rendering tasks for the game.
/// DirectX stuff that won't change much is hidden away in this file
/// so you won't have to keep looking at it.

#include <algorithm>

#include "gamerenderer.h"
#include "defines.h" 
#include "abort.h"
#include "imagefilenamelist.h"
#include "debug.h"
#include "sprite.h"
#include "object.h"
#include "objman.h"
#include "spriteman.h"
#include "SpriteSheet.h"
#include "Timer.h"
#include "TileMan.h"
#include "sound.h"
#include <iostream>

extern int g_nScreenWidth;
extern int g_nScreenHeight;
extern TileMan g_pTileManager;
extern BOOL g_bWireFrame;
extern CImageFileNameList g_cImageFileName;
extern CObjectManager g_cObjectManager;
extern CSpriteManager g_cSpriteManager; 
extern CTimer g_cTimer;
extern ShaderType g_nPixelShader;
extern HWND g_HwndApp;
extern MenuState g_nMenu_State;
extern SoundType g_nSoundState;
extern CSoundManager* g_pSoundManager; ///< The sound manager.
C3DSprite* g_pTileSprite[1000];

CGameRenderer::CGameRenderer(): m_bCameraDefaultMode(TRUE){
  m_cScreenText = nullptr;
  m_cScreenTextRed = nullptr;
  m_cScreenTextGreen = nullptr;
  m_nFrameCount = m_nLastFrameCountTime = 0;
  play = false;
  lastTile = NULL_TILE;

} //constructor

CGameRenderer::~CGameRenderer(){
  delete m_cScreenText;
  delete m_cScreenTextYellow;
  delete m_cScreenTextRed;
  delete m_cScreenTextGreen;

} //constructor

void CGameRenderer::checkMenuCollision(int x, int y) {
	BOOL sound = TRUE;
	//check if game is not in play
	if (g_nMenu_State == SELECTION_STATE) { //
											//check for Start Button
		if (x > 195 && x < 830) {
			if (y > 225 && y < 360) {
				OutputDebugString(("\nStart Button"));
				g_nMenu_State = PLAY_STATE;
			}//if
		}//if

		 //Check for Instruction Button
		if (x > 205 && x < 780) {
			if (y > 435 && y < 560) {
				OutputDebugString(("\nInstruction Button"));
				g_nMenu_State = INSTRUCTION_STATE;
			}//if
		}//if
		 //check for quit button
		if (x > 205 && x < 263) {
			if (y > 600 && y < 650) {
				OutputDebugString(("\nQuit Button"));
				//exit game
				g_nMenu_State = END_STATE;
			}
		}
		//check for toggle sound
		OutputDebugString(" Y: ");
		OutputDebugString(std::to_string(y).c_str());
		OutputDebugString("\n");
		if (x > 525 && x < 580) {
			if (y > 600 && y < 650) {
				OutputDebugString(("\nToggle Sound Button"));
				
				if (sound) {
					g_nSoundState = VOL_OFF;
					g_pSoundManager->volume(0);
					sound = FALSE;
				}
				else {
					g_nSoundState = VOL_ON;
					g_pSoundManager->volume(1);
					sound = TRUE;
				}
			}
		}


	}
	else if (g_nMenu_State == INSTRUCTION_STATE && x > 1) { g_nMenu_State = SELECTION_STATE; } //click to continue from instructions to game
}

/// Initialize the vertex and constant buffers for the background, that is, the
/// ground and the sky.

void CGameRenderer::InitBackground(){
  HRESULT hr;
  
  //load vertex buffer
  float w = 2.0f*g_nScreenWidth;
  float h = 2.0f*g_nScreenHeight;
  
  //vertex information, first triangle in clockwise order
  BILLBOARDVERTEX pVertexBufferData[6]; 
  pVertexBufferData[0].p = Vector3(w, 0, 0);
  pVertexBufferData[0].tu = 1.0f; pVertexBufferData[0].tv = 1.0f;

  pVertexBufferData[1].p = Vector3(0, 0, 0);
  pVertexBufferData[1].tu = 0.0f; pVertexBufferData[1].tv = 0.0f;

  pVertexBufferData[2].p = Vector3(w, 0, 1500);
  pVertexBufferData[2].tu = 1.0f; pVertexBufferData[2].tv = 1.0f;

  pVertexBufferData[3].p = Vector3(0, 0, 1500);
  pVertexBufferData[3].tu = 0.0f; pVertexBufferData[3].tv = 1.0f;

  pVertexBufferData[4].p = Vector3(w, h, 1500);
  pVertexBufferData[4].tu = 1.0f; pVertexBufferData[4].tv = 0.0f;

  pVertexBufferData[5].p = Vector3(0, h, 1500);
  pVertexBufferData[5].tu = 0.0f; pVertexBufferData[5].tv = 0.0f;
  
  //create vertex buffer for background
  m_pShader = new CShader(2, NUM_SHADERS);
    
  m_pShader->AddInputElementDesc(0, DXGI_FORMAT_R32G32B32_FLOAT, "POSITION");
  m_pShader->AddInputElementDesc(12, DXGI_FORMAT_R32G32_FLOAT,  "TEXCOORD");
  m_pShader->VSCreateAndCompile(L"VertexShader.hlsl", "main");
  m_pShader->PSCreateAndCompile(L"DoNothing.hlsl", "main", NULL_SHADER);
  m_pShader->PSCreateAndCompile(L"Grayscale.hlsl", "main", GRAYSCALE_SHADER);
  m_pShader->PSCreateAndCompile(L"Sepia.hlsl", "main", SEPIA_SHADER);
  m_pShader->PSCreateAndCompile(L"Ghost.hlsl", "main", GHOST_SHADER);
  m_pShader->PSCreateAndCompile(L"Psychadelic.hlsl", "main", PSYCHADELIC_SHADER);
  m_pShader->PSCreateAndCompile(L"Pixelate.hlsl", "main", PIXELATE_SHADER);
    
  // Create constant buffers.
  D3D11_BUFFER_DESC constantBufferDesc = { 0 };
  constantBufferDesc.ByteWidth = sizeof(ConstantBuffer); 
  constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  constantBufferDesc.CPUAccessFlags = 0;
  constantBufferDesc.MiscFlags = 0;
  constantBufferDesc.StructureByteStride = 0;
    
  m_pDev2->CreateBuffer(&constantBufferDesc, nullptr, &m_pConstantBuffer);
    
  D3D11_BUFFER_DESC VertexBufferDesc;
  VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  VertexBufferDesc.ByteWidth = sizeof(BILLBOARDVERTEX)* 6;
  VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  VertexBufferDesc.CPUAccessFlags = 0;
  VertexBufferDesc.MiscFlags = 0;
  VertexBufferDesc.StructureByteStride = 0;
    
  D3D11_SUBRESOURCE_DATA subresourceData;
  subresourceData.pSysMem = pVertexBufferData;
  subresourceData.SysMemPitch = 0;
  subresourceData.SysMemSlicePitch = 0;
    
  hr = m_pDev2->CreateBuffer(&VertexBufferDesc, &subresourceData, &m_pBackgroundVB);
} //InitBackground

/// Draw the game background.
/// \param x Camera x offset

void CGameRenderer::DrawBackground(float x){
  const float delta = 2.0f * g_nScreenWidth;
  float fQuantizeX = delta * (int)(x / delta - 1.0f) + g_nScreenWidth; //Quantized x coordinate

  UINT nVertexBufferOffset = 0;
  
  UINT nVertexBufferStride = sizeof(BILLBOARDVERTEX);
  m_pDC2->IASetVertexBuffers(0, 1, &m_pBackgroundVB, &nVertexBufferStride, &nVertexBufferOffset);
  m_pDC2->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
  m_pShader->SetShaders(g_nPixelShader);

  ConstantBuffer constantBufferData; ///< Constant buffer data for shader.

  constantBufferData.u0 = 0.0f;
  constantBufferData.u1 = 1.0f;
  constantBufferData.v0 = 0.0f;
  constantBufferData.v1 = 1.0f;

  //draw floor
  if(g_bWireFrame)
    m_pDC2->PSSetShaderResources(0, 1, &m_pWireframeTexture); //set wireframe texture
 
  SetWorldMatrix(Vector3(fQuantizeX, 0, 0));
  

  constantBufferData.wvp = CalculateWorldViewProjectionMatrix();
  m_pDC2->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &constantBufferData, 0, 0);
  m_pDC2->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);

  constantBufferData.wvp = CalculateWorldViewProjectionMatrix();
  m_pDC2->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &constantBufferData, 0, 0);
  m_pDC2->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
 // m_pDC2->Draw(4, 0);

  ////////////////////////////////////////////////////////////////////
  //draw backdrop
  if (!g_bWireFrame && play && !selectedTank) {
	  m_pDC2->PSSetShaderResources(0, 1, &m_pSPMenuTexture);
	  SetWorldMatrix(Vector3(fQuantizeX, 0, 0));
  }	else if (!g_bWireFrame && !play && !selectedTank) {
	  m_pDC2->PSSetShaderResources(0, 1, &m_pMenuTexture);

	  Vector3 pos, lookatpt;

	  if (m_bCameraDefaultMode) {
		  pos = Vector3(1050, 900, 0);
		  lookatpt = Vector3(1050, 900, 20000);
	  } //if
		
	  SetViewMatrix(pos, lookatpt);

	 
  } else if (!g_bWireFrame && !play) {
	  m_pDC2->PSSetShaderResources(0, 1, &m_pMenuTexture);
  }
  ////////////////////////////////////////////////////////////////////
    SetWorldMatrix(Vector3(fQuantizeX-(delta/2), 0, 0));

  constantBufferData.wvp = CalculateWorldViewProjectionMatrix();
  m_pDC2->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &constantBufferData, 0, 0);
  m_pDC2->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
  m_pDC2->Draw(4, 2);
  //SetWorldMatrix(Vector3(fQuantizeX - delta, 0, 0));

  float w = g_nScreenWidth / 2.0f;
  float h = g_nScreenHeight / 2.0f;

  ///clear the depth buffer
  m_pDC2->ClearDepthStencilView(m_pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

  //createGUI(fQuantizeX);

  constantBufferData.wvp = CalculateWorldViewProjectionMatrix();
  m_pDC2->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &constantBufferData, 0, 0);
  m_pDC2->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
} //DrawBackground
 
/// Load the background and sprite textures.

void CGameRenderer::LoadTextures(){ 
  //background
	int i = rand() % 5 + (1);
	LoadTexture(m_pMenuTexture, g_cImageFileName[0]);
        LoadTexture(m_pWireframeTexture, g_cImageFileName[6]);
	LoadTexture(m_pSPMenuTexture, g_cImageFileName[1]);
 //black for wireframe

  //sprites
  g_cSpriteManager.Load(TANK_OBJECT_O, "tank_4");
  g_cSpriteManager.Load(TANK_OBJECT, "tank_1");
  g_cSpriteManager.Load(BARREL_OBJECT,"barrel_green");
  g_cSpriteManager.Load(BARREL_OBJECT_O, "barrel_orange");

  g_cSpriteManager.Load(SILVER_BULLET, "silverBullet");
  g_cSpriteManager.Load(SEED_BULLET, "seedBullet");
  g_cSpriteManager.Load(GOLD_BULLET, "goldBullet");
  g_cSpriteManager.Load(NUKE_BULLET, "nukeBullet");


  g_cSpriteManager.Load(EXPLOSION_OBJECT, "explosion");
  g_cSpriteManager.Load(DIRT_EXPLOSION_OBJECT, "dirtExplosion");

 g_cSpriteManager.Load(END_TURN_OBJECT, "endTurn");

 g_cSpriteManager.Load(START_OBJECT, "start");
 g_cSpriteManager.Load(MENU_OBJECT, "menu");
 g_cSpriteManager.Load(SPLASH_OBJECT, "splash");
 g_cSpriteManager.Load(INSTRUCT_OBJECT, "instruct");
 
 g_cSpriteManager.Load(VICTORY_OBJECT1, "victory_p1");
 g_cSpriteManager.Load(VICTORY_OBJECT2, "victory_p2");

 g_cSpriteManager.Load(TILE_FLAT, "tile_flat");
 g_cSpriteManager.Load(TILE_HEIGHT_CONNECT_L, "tile_connectL");
 g_cSpriteManager.Load(TILE_HEIGHT_CONNECT_R, "tile_connectR");
 g_cSpriteManager.Load(TILE_MEDIUM_CURVE_LB, "tile_med_LBegin");
 g_cSpriteManager.Load(TILE_MEDIUM_CURVE_LE, "tile_med_LEnd");
 g_cSpriteManager.Load(TILE_MEDIUM_CURVE_RB, "tile_med_RBegin");
 g_cSpriteManager.Load(TILE_MEDIUM_CURVE_RE, "tile_med_REnd");
 g_cSpriteManager.Load(TILE_TALL_CURVE_L, "tile_tallL");
 g_cSpriteManager.Load(TILE_TALL_CURVE_R, "tile_tallR");
 g_cSpriteManager.Load(TILE_DIRT, "tile_dirt");
 g_cSpriteManager.Load(TILE_MOUND, "tile_mound");
 g_cSpriteManager.Load(TILE_LAVA, "tile_lava");
 
  g_cSpriteManager.Load(HUD_OBJECT,"hud");
  g_cSpriteManager.Load(CURR_TANK_OBJECT, "currTank");
  g_cSpriteManager.Load(ARROW_UP_OBJECT, "arrowUp");
  g_cSpriteManager.Load(ARROW_DOWN_OBJECT, "arrowDown");



  m_cScreenTextRed = new CSpriteSheet(21, 37);
  m_cScreenTextRed->Load("Images\\redNumberText.png");

  m_cScreenText = new CSpriteSheet(21, 37);
  m_cScreenText->Load("Images\\greenNumberText.png");

  m_cScreenTextGreen = new CSpriteSheet(21, 37);
  m_cScreenTextGreen->Load("Images\\greenNumberText.png");


  m_cScreenTextYellow = new CSpriteSheet(21, 37);
  m_cScreenTextYellow->Load("Images\\yellowNumberText.png");

} //LoadTextures

/// All textures used in the game are released - the release function is kind
/// of like a destructor for DirectX entities, which are COM objects.

void CGameRenderer::Release(){ 
  g_cSpriteManager.Release();
  SAFE_RELEASE(m_pWallTexture);
  SAFE_RELEASE(m_pFloorTexture);
  SAFE_RELEASE(m_pWireframeTexture);
  SAFE_RELEASE(m_pBackgroundVB);
  SAFE_RELEASE(m_cScreenText);
  SAFE_RELEASE(m_cScreenTextRed);
  SAFE_RELEASE(m_cScreenTextGreen);
  SAFE_RELEASE(m_cScreenTextYellow);

  SAFE_DELETE(m_pShader);

  CRenderer::Release();
} //Release

/// Draw text to HUD using a sprite sheet.
/// \param text The text to be drawn.bulletCollision
/// \param p Position at which to draw it.

void CGameRenderer::DrawHUDText(char* text, Vector3 p, ColorType color) {
	for(unsigned int i = 0; i < strlen(text); i++) {
		char c = text[i];
		if (c >= 'A' && c <= 'Z')
			p = m_cScreenText->Draw(p, 1, 48, c - 'A');
		else if (c >= 'a' && c <= 'z')
			p = m_cScreenText->Draw(p, 1, 95, c - 'a');
		else if (c >= '0' && c <= '9') {

			if(color== GREEN)
				p = m_cScreenTextGreen->Draw(p, 1, 1, c - '0');
			else if(color == RED)
				p = m_cScreenTextRed->Draw(p, 1, 1, c - '0');
			else if(color == YELLOW)
				p = m_cScreenTextYellow->Draw(p, 1, 1, c - '0');
			else {
				p = m_cScreenText->Draw(p, 1, 1, c - '0');

			}
		}
		else p = m_cScreenText->Draw(p, 1, 1, 10); //blank
	} //for
} //DrawHUDText

/// Draw the heads-up display.
/// \param text0 Text to draw at left of HUD.
/// \param text1 Text to draw at right of HUD.

void CGameRenderer::DrawHUD(char* text0, char* text1, char* text2, char* text3){
  float w = g_nScreenWidth/2.0f;
  float h = g_nScreenHeight/2.0f;
  //
  //switch to orthographic projection
  XMMATRIX tempProj = m_matProj;

  ///clear the depth buffer
  m_pDC2->ClearDepthStencilView(m_pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

  DrawHUDText("Time Left:", Vector3(1000, 1350.0f, 900.0f), GREEN);


  //arrowup
  C3DSprite* arrowUp = g_cSpriteManager.GetSprite(ARROW_UP_OBJECT);
  Vector3 aU = Vector3(505, 1940, arrowUpZ);
  arrowUp->Draw(aU, 0.0f, 0);

  Tank* tank = g_cObjectManager.GetPlayerObjectPtr();
  C3DSprite* bulletIcon = g_cSpriteManager.GetSprite(tank->bulletType[tank->bulletIndex]);
  Vector3 bI = Vector3(510, 1425, 2000);
  bulletIcon->Draw(bI, 0.0f, 0);

  C3DSprite* arrowDown = g_cSpriteManager.GetSprite(ARROW_DOWN_OBJECT);
  Vector3 aD = Vector3(505, 1740, arrowDownZ);
  arrowDown->Draw(aD, 0.0f, 0);

  if (arrowUpZ < 3450.0f) {
	  arrowUpZ = 3500.0f;
  }
  else {
	  arrowUpZ -= 1;
  }

  if (arrowDownZ > 3550.0f) {
	  arrowDownZ = 3500.0f;
  }
  else {
	  arrowDownZ += 1;
  }

  //draw the HUD background sprite	
  C3DSprite* hud = g_cSpriteManager.GetSprite(HUD_OBJECT);

  if (play) {
	  Vector3 p = Vector3(200, 1020, 500.0f);
	  hud->Draw(p, 0.0f, 0);
	  hud->Draw(Vector3(820, 1020, 500.0f), 0.0f, 0);
	 // hud->Draw(Vector3(510, 1040, 500.0f), 0.0f, 2001);

	//////////////////////////////////////////////////////////////////////////
	//						DRAW ENERGY / HEALTH ONTO HUD					//
	//////////////////////////////////////////////////////////////////////////

	  Tank* p1 = g_cObjectManager.GetPlayerObjectByTypePtr(PLAYER_ONE);
	  Tank* p2 = g_cObjectManager.GetPlayerObjectByTypePtr(PLAYER_TWO);
	  ColorType color= GREEN; 
	  ColorType colorEnergy = GREEN;

	  if (p1->health > 51) {
		  color = GREEN;
	  }
	  else if(p1->health < 51 && p1->health > 20){
		  color = YELLOW;
	  }
	  else {
		  color = RED;
	  }
	  DrawHUDText("Player One:", p + Vector3(420, 35.0f, -1.0), color);
	  
	  DrawHUDText(text0, p + Vector3(420, 0.0f, -1.0f), color);
	  if (p1->energy > 150) {
		  color = GREEN;
	  }
	  else if(p1->energy < 150 && p1-> energy > 75){
		  color = YELLOW;
	  }
	  else {
		  color = RED;
	  }
	  DrawHUDText(text1, p + Vector3(420, -35.0f, -1.0f), color);

	  if (p2->health > 51) {
		  color = GREEN;
	  }
	  else if(p2->health < 51 && p2->health > 21){
		  color = YELLOW;
	  }
	  else {
		  color = RED;
	  }
	  DrawHUDText("Player Two:", p + Vector3(1035, 35.0f, -1.0), color);

	  DrawHUDText(text2, p + Vector3(1040, 0.0f, -1.0f), color);


	  if (p2->energy > 150) {
		  color = GREEN;
	  }
	  else if(p2->energy < 150 && p2->energy > 75){
		  color = YELLOW;
	  }
	  else {
		  color = RED;
	  }
	  DrawHUDText(text3, p + Vector3(1040, -35.0f, -1.0f), color);
	  //////////////////////////////////////////////////////////////

  }
  
  //back to perspective projection 
  m_matProj = tempProj;
} //DrawHUD

void CGameRenderer::drawEnergy() {
	Tank* tank = g_cObjectManager.GetPlayerObjectPtr();

	HDC hdc = GetDC(g_HwndApp); //device context for screen
	RECT rc = {325,545,650,620}; //screen rectangle

//	//draw rectangle to screen
	SetDCBrushColor(hdc, 0x0025500);
	FillRect(hdc, &rc, (HBRUSH)GetStockObject(DC_BRUSH)); //fill in energy
}

/// Move all objects, then draw them.
/// \return TRUE if it succeeded

void CGameRenderer::ComposeFrame(){
  g_cObjectManager.move(); //move objects

  //set camera location
  Tank* p = g_cObjectManager.GetPlayerObjectPtr();
  Vector3 pos, lookatpt;
  
  if(m_bCameraDefaultMode){
    pos = Vector3(1024, 768, -350);
    lookatpt = Vector3(1024, 768, 1024);
  } //if
  else{
    pos = Vector3(1024, 1000, -3000);
    lookatpt = Vector3(1024, 700, 0);
  } //else
  
  SetViewMatrix(pos, lookatpt);
  
  //prepare to draw
  m_pDC2->OMSetRenderTargets(1, &m_pRTV, m_pDSV);
  float clearColor[] = { 1.0f, 1.0f, 1.0f, 0.0f };
  m_pDC2->ClearRenderTargetView(m_pRTV, clearColor);
  m_pDC2->ClearDepthStencilView(m_pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

  //draw
  DrawBackground(1024 + g_nScreenWidth/2.0f); //draw background
  //DrawPlatforms();  
  //loadMap();
  g_pTileManager.draw(); // draw tiles
  g_cObjectManager.draw(); //draw objects
  //draw text
  m_nFrameCount++;
  if(g_cTimer.elapsed(m_nLastFrameCountTime, 1000)){
    m_nDisplayedFrameCount = m_nFrameCount;
    m_nFrameCount = 0;
  } //if

  Tank* p1 = g_cObjectManager.GetPlayerObjectByTypePtr(PLAYER_ONE);
  Tank* p2 = g_cObjectManager.GetPlayerObjectByTypePtr(PLAYER_TWO);

  char buffer0[256], buffer1[256], buffer2[256], buffer3[256];

  if (p) {
		  sprintf_s(buffer0, "Health:%d", p1->health);
		  sprintf_s(buffer1, "Energy:%d", p1->energy);
		  sprintf_s(buffer2, "Health:%d", p2->health);
		  sprintf_s(buffer3, "Energy:%d", p2->energy);
		  DrawHUD(buffer0, buffer1, buffer2, buffer3);
  }
} //ComposeFrame
 
/// Compose a frame of animation and present it to the video card.

void CGameRenderer::setPlay(bool x) {
	play = x;
}


void CGameRenderer::ProcessFrame(){ 
  ComposeFrame(); //compose a frame of animation
  m_pSwapChain2->Present(1, 0); //present it
} //ProcessFrame

/// Toggle between eagle-eye camera (camera pulled back far enough to see
/// backdrop) and the normal game camera.

void CGameRenderer::FlipCameraMode(){
  m_bCameraDefaultMode = !m_bCameraDefaultMode; 

} //FlipCameraMode
