#include "Application.h"

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // check for window closing
    switch (message)
    {
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }
    break;
    }

    // handle messages that the switch statement did not handle
    return DefWindowProc(hWnd, message, wParam, lParam);
}

Application::Application(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    HWND hWnd = nullptr;
    WNDCLASSEX wc = { };

    // fill in the struct with required information
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = "DirectX Window";

    // register the window class
    RegisterClassEx(&wc);

    // set and adjust the size
    RECT wr = { 0, 0, WIDTH, HEIGHT };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    // create the window and use the result as the handle
    hWnd = CreateWindowEx(NULL,
        "DirectX Window",           // name of the window class
        "DirectX 11 Playground",    // title of the window
        WS_OVERLAPPEDWINDOW,        // window style
        200,                        // x-position of the window
        100,                        // y-position of the window
        wr.right - wr.left,         // width of the window
        wr.bottom - wr.top,         // height of the window
        NULL,                       // we have no parent window, NULL
        NULL,                       // we aren't using menus, NULL
        hInstance,                  // application handle
        NULL);                      // used with multiple windows, NULL

    ShowWindow(hWnd, nCmdShow);

	Initialize(hWnd);
}

void Application::InitRenderData()
{

    HRESULT result = S_OK;
    // initialize render targets
    // half res for RT 1 and RT 2, while RT 0 has full resolution
    const UINT widths[NUM_RENDERTARGETS] = { WIDTH, WIDTH / 2, WIDTH / 2 };
    const UINT heights[NUM_RENDERTARGETS] = { HEIGHT, HEIGHT / 2, HEIGHT / 2 };

    for (UINT32 i = 0; i < NUM_RENDERTARGETS; ++i)
    {
        RenderTarget& renderTarget = renderTargets[i];

        D3D11_TEXTURE2D_DESC textureDesc;
        D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;

        ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
        ZeroMemory(&renderTargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
        ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
        ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));

        // 1. Create texture
        textureDesc.Width = widths[i];
        textureDesc.Height = heights[i];
        textureDesc.MipLevels = 1;
        textureDesc.ArraySize = 1;
        textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        textureDesc.CPUAccessFlags = 0;
        textureDesc.MiscFlags = 0;

        result = device->CreateTexture2D(&textureDesc, NULL, &renderTarget.renderTargetTexture);
        if (FAILED(result))
        {
            std::cerr << "Failed to create render target texture\n";
            exit(-1);
        }

        // 2. Create render target view
        renderTargetViewDesc.Format = textureDesc.Format;
        renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        renderTargetViewDesc.Texture2D.MipSlice = 0;

        result = device->CreateRenderTargetView(renderTarget.renderTargetTexture, &renderTargetViewDesc, &renderTarget.renderTargetView);
        if (FAILED(result))
        {
            std::cerr << "Failed to create render target view\n";
            exit(-1);
        }

        // 3. Create SRV
        srvDesc.Format = textureDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = 1;

        result = device->CreateShaderResourceView(renderTarget.renderTargetTexture, &srvDesc, &renderTarget.shaderResourceView);
        if (FAILED(result))
        {
            std::cerr << "Failed to create render target texture SRV\n";
            exit(-1);
        }

        // 4. Create UAV
        uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
        uavDesc.Texture2D.MipSlice = 0;

        result = device->CreateUnorderedAccessView(renderTarget.renderTargetTexture, &uavDesc, &renderTarget.unorderedAccessView);
        if (FAILED(result))
        {
            std::cerr << "Failed to create render target texture UAV\n";
            exit(-1);
        }
    }

    // intialize depth-stencil target
    {
        D3D11_TEXTURE2D_DESC descDepth;
        ZeroMemory(&descDepth, sizeof(D3D11_TEXTURE2D_DESC));

        descDepth.Width = WIDTH;
        descDepth.Height = HEIGHT;
        descDepth.MipLevels = 1;
        descDepth.ArraySize = 1;
        descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        descDepth.SampleDesc.Count = 1;
        descDepth.SampleDesc.Quality = 0;
        descDepth.Usage = D3D11_USAGE_DEFAULT;
        descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        descDepth.CPUAccessFlags = 0;
        descDepth.MiscFlags = 0;

        result = device->CreateTexture2D(&descDepth, NULL, &depthStencilTarget.dsTexture);
        if (FAILED(result))
        {
            std::cerr << "Failed to create depth/stencil texture\n";
            exit(-1);
        }

        D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
        ZeroMemory(&descDSV, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

        descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        descDSV.Texture2D.MipSlice = 0;

        result = device->CreateDepthStencilView(depthStencilTarget.dsTexture, &descDSV, &depthStencilTarget.dsView);
        if (FAILED(result))
        {
            std::cerr << "Failed to create depth/stencil view\n";
            exit(-1);
        }
    }

    // initialize depth-stencil state
    {
        D3D11_DEPTH_STENCIL_DESC dsDesc;
        ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

        dsDesc.DepthEnable = true;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
        dsDesc.StencilEnable = false;

        result = device->CreateDepthStencilState(&dsDesc, &depthStencilStateWithDepthTest);
        if (FAILED(result))
        {
            std::cerr << "Failed to create depth/stencil state\n";
            exit(-1);
        }

        dsDesc.DepthEnable = false;

        result = device->CreateDepthStencilState(&dsDesc, &depthStencilStateWithoutDepthTest);
        if (FAILED(result))
        {
            std::cerr << "Failed to create depth/stencil state\n";
            exit(-1);
        }
    }

    // initialize rasterizer state
    {
        // Setup the raster description which will determine how and what polygons will be drawn.
        D3D11_RASTERIZER_DESC rasterDesc;
        ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));

        rasterDesc.AntialiasedLineEnable = false;
        rasterDesc.CullMode = D3D11_CULL_BACK;
        rasterDesc.DepthBias = 0;
        rasterDesc.DepthBiasClamp = 0.0f;
        rasterDesc.DepthClipEnable = true;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
        rasterDesc.FrontCounterClockwise = false;
        rasterDesc.MultisampleEnable = false;
        rasterDesc.ScissorEnable = false;
        rasterDesc.SlopeScaledDepthBias = 0.f;

        // Create the rasterizer state from the description we just filled out.
        result = device->CreateRasterizerState(&rasterDesc, &defaultRasterizerState);
        if (FAILED(result))
        {
            std::cerr << "Failed to create rasterizer state\n";
            exit(-1);
        }
        deviceContext->RSSetState(defaultRasterizerState);
    }

    //initialize model vertex buffer
    {
        std::vector<VertexPosNormal> meshData;
        if (!LoadObjFile("data/mesh.obj", meshData))
        {
            std::cerr << "Loading Obj Mesh failed";
            exit(-1);
        }

        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));

        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth = static_cast<UINT>(sizeof(VertexPosNormal) * meshData.size());
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        // create the buffer
        result = device->CreateBuffer(&bd, NULL, &objModelMesh.vertexBuffer);
        if (FAILED(result))
        {
            std::cerr << "Failed to create model vertex buffer\n";
            exit(-1);
        }

        // copy vertex data to buffer
        D3D11_MAPPED_SUBRESOURCE ms;
        deviceContext->Map(objModelMesh.vertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
        memcpy(ms.pData, meshData.data(), sizeof(VertexPosNormal) * meshData.size());
        deviceContext->Unmap(objModelMesh.vertexBuffer, NULL);

        // create input vertex layout
        D3D11_INPUT_ELEMENT_DESC ied[] =
        {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
        };

        result = device->CreateInputLayout(ied, 2, vsPhongShader->GetShaderBlob()->GetBufferPointer(), vsPhongShader->GetShaderBlob()->GetBufferSize(), &objModelMesh.vertexLayout);
        if (FAILED(result))
        {
            std::cerr << "Failed to create model input layout\n";
            exit(-1);
        }

        objModelMesh.vertexCount = static_cast<UINT>(meshData.size());
        objModelMesh.stride = static_cast<UINT>(sizeof(VertexPosNormal));
        objModelMesh.offset = 0;
        objModelMesh.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    }

    // initialize screen aligned quad
    {
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));

        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth = static_cast<UINT>(sizeof(VertexPosTexCoord) * ScreenAlignedQuad.size());
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        // create the buffer
        result = device->CreateBuffer(&bd, NULL, &screenAlignedQuadMesh.vertexBuffer);
        if (FAILED(result))
        {
            std::cerr << "Failed to create screen aligned quad vertex buffer\n";
            exit(-1);
        }

        // copy vertex data to buffer
        D3D11_MAPPED_SUBRESOURCE ms;
        deviceContext->Map(screenAlignedQuadMesh.vertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
        memcpy(ms.pData, ScreenAlignedQuad.data(), sizeof(VertexPosTexCoord) * ScreenAlignedQuad.size());
        deviceContext->Unmap(screenAlignedQuadMesh.vertexBuffer, NULL);

        // create input vertex layout
        D3D11_INPUT_ELEMENT_DESC ied[] =
        {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
        };

        result = device->CreateInputLayout(ied, 2, vsQuadCompositeShader->GetShaderBlob()->GetBufferPointer(), vsQuadCompositeShader->GetShaderBlob()->GetBufferSize(), &screenAlignedQuadMesh.vertexLayout);
        if (FAILED(result))
        {
            std::cerr << "Failed to create screen aligned quad input layout\n";
            exit(-1);
        }

        screenAlignedQuadMesh.vertexCount = static_cast<UINT>(ScreenAlignedQuad.size());
        screenAlignedQuadMesh.stride = static_cast<UINT>(sizeof(VertexPosTexCoord));
        screenAlignedQuadMesh.offset = 0;
        screenAlignedQuadMesh.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    }

    // initialize transforms
    {
        transforms.model = DirectX::XMMatrixIdentity();
        transforms.view = DirectX::XMMatrixIdentity();
        transforms.proj = DirectX::XMMatrixIdentity();
    }

    // create transformation constant buffer
    {
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(CD3D11_BUFFER_DESC));

        bd.ByteWidth = sizeof(Transformations);
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        // create the buffer
        result = device->CreateBuffer(&bd, NULL, &transformConstantBuffer);
        if (FAILED(result))
        {
            std::cerr << "Failed to create transform constant buffer\n";
            exit(-1);
        }
    }

    // compositing constant buffer
    {
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(CD3D11_BUFFER_DESC));

        bd.ByteWidth = sizeof(CompositeParams);
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        // create the buffer
        result = device->CreateBuffer(&bd, NULL, &compositionConstantBuffer);
        if (FAILED(result))
        {
            std::cerr << "Failed to create composition constant buffer\n";
            exit(-1);
        }
    }

    //default texture sampler
    {
        D3D11_SAMPLER_DESC sampDesc = { };
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sampDesc.MinLOD = 0;
        sampDesc.MaxLOD = 1;

        result = device->CreateSamplerState(&sampDesc, &defaultSamplerState);
        if (FAILED(result))
        {
            std::cerr << "Failed to create texture sampler\n";
            exit(-1);
        }
    }

    // Material and light source
    {
        // light values are updated during UpdateTick()
        lightSource.lightPosition = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);
        lightSource.lightColorAndPower = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);

        material.ambient = DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f);
        material.diffuse = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);
        material.specularAndShininess = DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 24.f);

        {
            D3D11_BUFFER_DESC bd;
            ZeroMemory(&bd, sizeof(CD3D11_BUFFER_DESC));

            bd.ByteWidth = sizeof(LightSource);
            bd.Usage = D3D11_USAGE_DYNAMIC;
            bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            // create the buffer
            result = device->CreateBuffer(&bd, NULL, &lightSourceConstantBuffer);
            if (FAILED(result))
            {
                std::cerr << "Failed to create light source constant buffer\n";
                exit(-1);
            }
        }

        {
            D3D11_BUFFER_DESC bd;
            ZeroMemory(&bd, sizeof(CD3D11_BUFFER_DESC));

            bd.ByteWidth = sizeof(Material);
            bd.Usage = D3D11_USAGE_DYNAMIC;
            bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            // create the buffer
            result = device->CreateBuffer(&bd, NULL, &materialConstantBuffer);
            if (FAILED(result))
            {
                std::cerr << "Failed to create material constant buffer\n";
                exit(-1);
            }

            D3D11_MAPPED_SUBRESOURCE ms;
            deviceContext->Map(materialConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
            memcpy(ms.pData, &material, sizeof(Material));
            deviceContext->Unmap(materialConstantBuffer, NULL);
        }
    }

    // compute shader constant buffers
    {
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(CD3D11_BUFFER_DESC));

        bd.ByteWidth = sizeof(ThresholdParams);
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        // create the buffer
        result = device->CreateBuffer(&bd, NULL, &thresholdConstantBuffer);
        if (FAILED(result))
        {
            std::cerr << "Failed to create threshold constant buffer\n";
            exit(-1);
        }
    }

    // compute blur parameters
    {
        blurParams.radius = GAUSSIAN_RADIUS;
        blurParams.direction = 0;

        // compute Gaussian kernel
        float sigma = 10.f;
        float sigmaRcp = 1.f / sigma;
        float twoSigmaSq = 2 * sigma * sigma;

        float sum = 0.f;
        for (size_t i = 0; i <= GAUSSIAN_RADIUS; ++i)
        {
            // we omit the normalization factor here for the discrete version and normalize using the sum afterwards
            blurParams.coefficients[i] = (1.f / sigma) * std::expf(-static_cast<float>(i * i) / twoSigmaSq);
            // we use each entry twice since we only compute one half of the curve
            sum += 2 * blurParams.coefficients[i];
        }
        // the center (index 0) has been counted twice, so we subtract it once
        sum -= blurParams.coefficients[0];

        // we normalize all entries using the sum so that the entire kernel gives us a sum of coefficients = 0
        float normalizationFactor = 1.f / sum;
        for (size_t i = 0; i <= GAUSSIAN_RADIUS; ++i)
        {
            blurParams.coefficients[i] *= normalizationFactor;
        }
    }

    {
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(CD3D11_BUFFER_DESC));

        bd.ByteWidth = sizeof(BlurParams);
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        // create the buffer
        result = device->CreateBuffer(&bd, NULL, &blurConstantBuffer);
        if (FAILED(result))
        {
            std::cerr << "Failed to create blur constant buffer\n";
            exit(-1);
        }
    }

    // set the viewport (note: since this does not change, it is sufficient to do this once)
    D3D11_VIEWPORT viewport = { };
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = static_cast<float>(WIDTH);
    viewport.Height = static_cast<float>(HEIGHT);
    viewport.MinDepth = 0.f;
    viewport.MaxDepth = 1.f;

    deviceContext->RSSetViewports(1, &viewport);
}


void Application::Run() {
    InitRenderData();

    // main loop
    timer.Start();

    MSG msg = { };
    while (true)
    {
        std::cout << "Running main loop..." << std::endl;
        // Check to see if any messages are waiting in the queue
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // translate and send the message to the WindowProc function
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            // handle quit
            if (msg.message == WM_QUIT)
            {
                break;
            }
        }
        else
        {
            // get time elapsed since last frame start
            timer.Stop();
            float elapsedMilliseconds = timer.GetElapsedTimeMilliseconds();
            timer.Start();

            // upate and render
            Update(elapsedMilliseconds);

            Render();
        }
    }

    // shutdown
    CleanupRenderData();
    Cleanup();
}

bool Application::Initialize(HWND hWnd)
{
    UINT createDeviceFlags = 0;
#if defined(_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    D3D_FEATURE_LEVEL featureLevelCreated;
    HRESULT hr = D3D11CreateDevice(
        nullptr, // Adapter
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr, // Software
        createDeviceFlags, // Flags
        featureLevels, // Feature levels
        ARRAYSIZE(featureLevels), // Feature levels count
        D3D11_SDK_VERSION,
        &device,
        &featureLevelCreated, // Feature level
        &deviceContext
    );
    DXGI_SWAP_CHAIN_DESC scd;
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

    scd.BufferCount = 1;
    scd.BufferDesc.Width = WIDTH;
    scd.BufferDesc.Height = HEIGHT;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.RefreshRate.Numerator = 60;
    scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hWnd;
    scd.SampleDesc.Count = 1;
    scd.SampleDesc.Quality = 0;
    scd.Windowed = TRUE;
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    scd.Flags = 0;
    // create a device, device context and swap chain

    IDXGIFactory* pFactory = nullptr;
    IDXGIDevice* pDXGIDevice = nullptr;
    IDXGIAdapter* pAdapter = nullptr;
// Get the DXGI factory from the device
    device->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDXGIDevice);
    pDXGIDevice->GetAdapter(&pAdapter);
    pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);

    /*auto hr = D3D11CreateDeviceAndSwapChain(NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        NULL,
        featureLevels,
        ARRAYSIZE(featureLevels),
        D3D11_SDK_VERSION,
        &scd,
        &swapchain,
        &device,
        NULL,
        &deviceContext);*/

    hr = pFactory->CreateSwapChain(device, &scd, &swapchain);
    if (FAILED(hr))
    {
        std::cerr << "Failed to create Direct3D device and swap chain: " << std::hex << hr << std::endl;
        return false; // Return false if initialization failed
    }

    if (pDXGIDevice) pDXGIDevice->Release();
    if (pAdapter) pAdapter->Release();
    if (pFactory) pFactory->Release();
    // get the address of the back buffer
    ID3D11Texture2D* pBackBuffer = nullptr;
    swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (pBackBuffer != nullptr)
    {
        // use the back buffer address to create the render target
        device->CreateRenderTargetView(pBackBuffer, nullptr, &backbuffer);
        pBackBuffer->Release();
    }
    else
    {
        std::cerr << "Could not obtain backbuffer from swapchain";
        exit(-1);
    }

    // build shaders
    ID3DBlob* errorBlob = nullptr;

    // model shader
    {
        vsPhongShader = new VertexShaderNew("PhongShader", "shaders/phong.hlsl", device, deviceContext);
        ID3D11VertexShader* vsTempShader = nullptr;
        device->CreateVertexShader(vsPhongShader->GetShaderBlob()->GetBufferPointer(), vsPhongShader->GetShaderBlob()->GetBufferSize(), NULL, &vsTempShader);
        vsPhongShader->SetShader(vsTempShader);

        psPhongShader = new PixelShaderNew("PhongShader", "shaders/phong.hlsl", device, deviceContext);
        ID3D11PixelShader* psTempShader = nullptr;
        device->CreatePixelShader(psPhongShader->GetShaderBlob()->GetBufferPointer(), psPhongShader->GetShaderBlob()->GetBufferSize(), NULL, &psTempShader);
        psPhongShader->SetShader(psTempShader);
    }

    // textured screen-aligned quad shader
    {
        vsQuadCompositeShader = new VertexShaderNew("QuadCompositeShader", "shaders/quadcomposite.hlsl", device, deviceContext);
        ID3D11VertexShader* vsTempShader = nullptr;
        device->CreateVertexShader(vsQuadCompositeShader->GetShaderBlob()->GetBufferPointer(), vsQuadCompositeShader->GetShaderBlob()->GetBufferSize(), NULL, &vsTempShader);
        vsQuadCompositeShader->SetShader(vsTempShader);

        psQuadCompositeShader = new PixelShaderNew("QuadCompositeShader", "shaders/quadcomposite.hlsl", device, deviceContext);
        ID3D11PixelShader* psTempShader = nullptr;
        device->CreatePixelShader(psQuadCompositeShader->GetShaderBlob()->GetBufferPointer(), psQuadCompositeShader->GetShaderBlob()->GetBufferSize(), NULL, &psTempShader);
        psQuadCompositeShader->SetShader(psTempShader);
    }

    // compute shaders
    {
        thresholdDownsampleShader_new = new ComputeShaderNew("ThresholdDownsample", "shaders/thresholddownsample.hlsl", device, deviceContext);
        ID3D11ComputeShader* tempShader = nullptr;
        device->CreateComputeShader(thresholdDownsampleShader_new->GetShaderBlob()->GetBufferPointer(), thresholdDownsampleShader_new->GetShaderBlob()->GetBufferSize(), NULL, &tempShader);
        thresholdDownsampleShader_new->SetShader(tempShader);
    }
    {
        blurShader_new = new ComputeShaderNew("Blur", "shaders/blur.hlsl", device, deviceContext);
        ID3D11ComputeShader* tempShader = nullptr;
        device->CreateComputeShader(blurShader_new->GetShaderBlob()->GetBufferPointer(), blurShader_new->GetShaderBlob()->GetBufferSize(), NULL, &tempShader);
        blurShader_new->SetShader(tempShader);
    }
	return true; // Return true if successful, false otherwise
}

void Application::Update(float deltaTime)
{
    // approx. 10 seconds for one full rotation
    constexpr float millisecondsToAngle = 0.0001f * 6.28f;

    // update model transform
    transforms.model = DirectX::XMMatrixMultiply(transforms.model, DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationY(deltaTime * millisecondsToAngle)));
    // update view and projection transforms
    // note: it is unnecessary to do this since these do not change in the application
    DirectX::XMVECTOR cameraPos = DirectX::XMVectorSet(0.f, 0.4f, 0.75f, 1.f);
    DirectX::XMVECTOR cameraFokus = DirectX::XMVectorSet(0.f, 0.f, 0.f, 1.f);
    DirectX::XMVECTOR cameraUp = DirectX::XMVectorSet(0.f, 1.f, 0.f, 1.f);
    transforms.view = DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(cameraPos, cameraFokus, cameraUp));
    transforms.proj = DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(1.5f, static_cast<float>(WIDTH) / static_cast<float>(HEIGHT), 0.01f, 100.f));

    // update light source (same as before: since it is constant, we do not need to update it every frame)
    DirectX::XMVECTOR lightWorldPos = DirectX::XMVectorSet(-1.5f, 1.5f, 1.5f, 1.f);
    DirectX::XMVECTOR lightViewPos = DirectX::XMVector4Transform(lightWorldPos, transforms.view);
    DirectX::XMStoreFloat4(&lightSource.lightPosition, lightViewPos);

    lightSource.lightColorAndPower = DirectX::XMFLOAT4(1.f, 1.f, 0.7f, 4.5f);
}

void Application::Render()
{
    constexpr ID3D11RenderTargetView* NULL_RT = nullptr;
    constexpr ID3D11ShaderResourceView* NULL_SRV = nullptr;
    constexpr ID3D11UnorderedAccessView* NULL_UAV = nullptr;
    constexpr UINT NO_OFFSET = -1;

    // first pass: render the mesh with Blinn-Phong lighting
    {
        // clear and set up the render target
        constexpr float backgroundColor[4] = { 0.f, 0.f, 0.f, 1.f };
        deviceContext->ClearRenderTargetView(renderTargets[0].renderTargetView, backgroundColor);
        //deviceContext->ClearRenderTargetView(backbuffer, backgroundColor);
        deviceContext->ClearDepthStencilView(depthStencilTarget.dsView, D3D11_CLEAR_DEPTH, 1.f, 0);
        deviceContext->OMSetRenderTargets(1, &renderTargets[0].renderTargetView, depthStencilTarget.dsView);
        //deviceContext->OMSetRenderTargets(1, &backbuffer, nullptr);

        // set depth-stencil state
        deviceContext->OMSetDepthStencilState(depthStencilStateWithDepthTest, 0);

        // set the shader objects
        deviceContext->VSSetShader(vsPhongShader->GetShader(), 0, 0);
        deviceContext->PSSetShader(psPhongShader->GetShader(), 0, 0);

        // set vertex input layout, vertex buffer and primitive topology
        deviceContext->IASetInputLayout(objModelMesh.vertexLayout);
        deviceContext->IASetVertexBuffers(0, 1, &objModelMesh.vertexBuffer, &objModelMesh.stride, &objModelMesh.offset);
        deviceContext->IASetPrimitiveTopology(objModelMesh.topology);

        // update the transformation matrices in the constant buffer
        {
            D3D11_MAPPED_SUBRESOURCE ms;
            auto result = deviceContext->Map(transformConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
            if (SUCCEEDED(result))
            {
                memcpy(ms.pData, &transforms, sizeof(Transformations));
                deviceContext->Unmap(transformConstantBuffer, 0);
            } else {
                std::cerr << "Failed to map transformation constant buffer: " << std::hex << result << std::endl;
            }
        }
        // update light source constant buffer
        {
            D3D11_MAPPED_SUBRESOURCE ms;
            auto result = deviceContext->Map(lightSourceConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
            if (SUCCEEDED(result))
            {
                memcpy(ms.pData, &lightSource, sizeof(LightSource));
                deviceContext->Unmap(lightSourceConstantBuffer, 0);
            }else{
                std::cerr << "Failed to map light source constant buffer: " << std::hex << result << std::endl;
            }
        }

        // set the constant buffers for transformations, material, and light source
        std::array<ID3D11Buffer*, 3> constantBuffers{ transformConstantBuffer, lightSourceConstantBuffer, materialConstantBuffer };
        deviceContext->VSSetConstantBuffers(0, 1, &constantBuffers[0]);
        deviceContext->PSSetConstantBuffers(0, 2, &constantBuffers[1]);

        // draw the mesh
        deviceContext->Draw(objModelMesh.vertexCount, 0);

        // unbind render target and turn depth test off
        deviceContext->OMSetRenderTargets(1, &NULL_RT, nullptr);
        deviceContext->OMSetDepthStencilState(depthStencilStateWithoutDepthTest, 0);
    }
    /*auto result = swapchain->Present(1, 0);
    if (FAILED(result))
    {
         std::cout << "Present failed. HRESULT: 0x" << std::hex << result << std::endl;
        HRESULT reason = device->GetDeviceRemovedReason();
        if (reason == DXGI_ERROR_DEVICE_REMOVED)
        {
            std::cerr << "Device was removed: " << std::hex << reason << std::dec << "\n";
        }
       std::cerr << "Failed to create render target texture UAV\n";
    }
    deviceContext->OMSetRenderTargets(1, &NULL_RT, nullptr);
    std::cout<< "present"<<std::endl;
    return;
    */

    // use compute shaders for post-processing

    // 1. downsample to half resolution and threshold
    {
        ThresholdParams thresholdParams = { 0.5f };
        {
            D3D11_MAPPED_SUBRESOURCE ms;
            auto result =  deviceContext->Map(thresholdConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
            if (SUCCEEDED(result))
            {
                memcpy(ms.pData, &thresholdParams, sizeof(ThresholdParams));
                deviceContext->Unmap(thresholdConstantBuffer, 0);
            } else {
                std::cerr << "Failed to map threshold constant buffer: " << std::hex << result << std::endl;
            }
        }

        deviceContext->CSSetShader(thresholdDownsampleShader_new->GetShader(), 0, 0);
        deviceContext->CSSetShaderResources(0, 1, &renderTargets[0].shaderResourceView);
        deviceContext->CSSetUnorderedAccessViews(0, 1, &renderTargets[1].unorderedAccessView, &NO_OFFSET);
        deviceContext->CSSetConstantBuffers(0, 1, &thresholdConstantBuffer);

        deviceContext->Dispatch(WIDTH / 16, HEIGHT / 16, 1);

        // unbind UAV and SRVs
        deviceContext->CSSetShaderResources(0, 1, &NULL_SRV);
        deviceContext->CSSetUnorderedAccessViews(0, 1, &NULL_UAV, &NO_OFFSET);
    }


    // 2. Gaussian blur (in two passes) - use renderTargets[1] and renderTargets[2] with half resolution
    deviceContext->CSSetShader(blurShader_new->GetShader(), 0, 0);
    std::array<ID3D11ShaderResourceView*, 2> csSRVs = { renderTargets[1].shaderResourceView, renderTargets[2].shaderResourceView };
    std::array<ID3D11UnorderedAccessView*, 2> csUAVs = { renderTargets[2].unorderedAccessView, renderTargets[1].unorderedAccessView };
    for (UINT direction = 0; direction < 2; ++direction)
    {
        blurParams.direction = direction;
        {
            D3D11_MAPPED_SUBRESOURCE ms;
            size_t test = sizeof(BlurParams);
            auto result = deviceContext->Map(blurConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
            if (SUCCEEDED(result))
            {
                memcpy(ms.pData, &blurParams, sizeof(BlurParams));
                deviceContext->Unmap(blurConstantBuffer, 0);
            } else {
                std::cerr << "Failed to map blur constant buffer: " << std::hex << result << std::endl;
            }
        }

        deviceContext->CSSetShaderResources(0, 1, &csSRVs[direction]);
        deviceContext->CSSetUnorderedAccessViews(0, 1, &csUAVs[direction], &NO_OFFSET);

        deviceContext->CSSetConstantBuffers(0, 1, &blurConstantBuffer);

        deviceContext->Dispatch(WIDTH / 16, HEIGHT / 16, 1);

        // unbind UAV and SRVs
        deviceContext->CSSetShaderResources(0, 1, &NULL_SRV);
        deviceContext->CSSetUnorderedAccessViews(0, 1, &NULL_UAV, &NO_OFFSET);
    }

    // Composite blurred half-res image with original image in pixel shader by rendering a fullscreen quad
    // to the back buffer (no need to clear since we render a fullscreen quad without depth test)
    deviceContext->OMSetRenderTargets(1, &backbuffer, NULL);

    deviceContext->VSSetShader(vsQuadCompositeShader->GetShader(), 0, 0);
    deviceContext->PSSetShader(psQuadCompositeShader->GetShader(), 0, 0);

    deviceContext->IASetInputLayout(screenAlignedQuadMesh.vertexLayout);
    deviceContext->IASetVertexBuffers(0, 1, &screenAlignedQuadMesh.vertexBuffer, &screenAlignedQuadMesh.stride, &screenAlignedQuadMesh.offset);
    deviceContext->IASetPrimitiveTopology(screenAlignedQuadMesh.topology);

    std::array<ID3D11ShaderResourceView*, 2> compositeSRVs = { renderTargets[0].shaderResourceView, renderTargets[1].shaderResourceView };
    deviceContext->PSSetShaderResources(0, 2, &compositeSRVs[0]);

    deviceContext->PSSetSamplers(0, 1, &defaultSamplerState);

    CompositeParams compParams = { 0.75f };
    {
        D3D11_MAPPED_SUBRESOURCE ms;
        auto result = deviceContext->Map(compositionConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
        if (SUCCEEDED(result))
        {
            memcpy(ms.pData, &compParams, sizeof(CompositeParams));
            deviceContext->Unmap(compositionConstantBuffer, 0);
        } else {
            std::cerr << "Failed to map composition constant buffer: " << std::hex << result << std::endl;
        }
    }
    deviceContext->PSSetConstantBuffers(0, 1, &compositionConstantBuffer);

    deviceContext->Draw(screenAlignedQuadMesh.vertexCount, 0);

    //unbind SRVs
    deviceContext->PSSetShaderResources(0, 1, &NULL_SRV);

    // switch the back buffer and the front buffer
    auto result = swapchain->Present(0, 0);
    if (FAILED(result))
    {
         std::cout << "Present failed. HRESULT: 0x" << std::hex << result << std::endl;
        HRESULT reason = device->GetDeviceRemovedReason();
        if (reason == DXGI_ERROR_DEVICE_REMOVED)
        {
            std::cerr << "Device was removed: " << std::hex << reason << std::dec << "\n";
        }
       std::cerr << "Failed to create render target texture UAV\n";
    }
    std::cout<< "present"<<std::endl;
}

void Application::CleanupRenderData()
{
    defaultRasterizerState->Release();
    defaultSamplerState->Release();

    // constant buffers
    transformConstantBuffer->Release();
    lightSourceConstantBuffer->Release();
    materialConstantBuffer->Release();

    compositionConstantBuffer->Release();
    blurConstantBuffer->Release();
    thresholdConstantBuffer->Release();


    // meshes
    objModelMesh.vertexBuffer->Release();
    objModelMesh.vertexLayout->Release();

    screenAlignedQuadMesh.vertexBuffer->Release();
    screenAlignedQuadMesh.vertexLayout->Release();

    // depth-stencil states
    depthStencilStateWithDepthTest->Release();
    depthStencilStateWithoutDepthTest->Release();

    // render targets
	for (size_t i = 0; i < NUM_RENDERTARGETS; ++i)
	{
		if (renderTargets[i].renderTargetTexture) renderTargets[i].renderTargetTexture->Release();
		if (renderTargets[i].renderTargetView) renderTargets[i].renderTargetView->Release();
		if (renderTargets[i].shaderResourceView) renderTargets[i].shaderResourceView->Release();
		if (renderTargets[i].unorderedAccessView) renderTargets[i].unorderedAccessView->Release();
	}

    // depth-stencil target
	if (depthStencilTarget.dsTexture) depthStencilTarget.dsTexture->Release();
	if (depthStencilTarget.dsView) depthStencilTarget.dsView->Release();
}

void Application::HandleInput()
{
	// Handle input events (keyboard, mouse, etc.)
}

void Application::Cleanup()
{
    thresholdDownsampleShader_new->GetShaderBlob()->Release();
    thresholdDownsampleShader_new->GetShader()->Release();

    blurShader_new->GetShaderBlob()->Release();
    blurShader_new->GetShader()->Release();

    vsPhongShader->GetShaderBlob()->Release();
    psPhongShader->GetShader()->Release();
    vsPhongShader->GetShaderBlob()->Release();
    psPhongShader->GetShader()->Release();

    vsQuadCompositeShader->GetShaderBlob()->Release();
    psQuadCompositeShader->GetShaderBlob()->Release();
    vsQuadCompositeShader->GetShader()->Release();
    psQuadCompositeShader->GetShader()->Release();

    swapchain->Release();
    backbuffer->Release();
    device->Release();
    deviceContext->Release();
}