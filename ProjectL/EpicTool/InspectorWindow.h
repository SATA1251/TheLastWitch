#pragma once
#include <vector>
#include <string>
#include "IWindow.h"


namespace KunrealEngine
{
    class GameObject;
    class Transform;
    class MeshRenderer;
    class Compoent;
    class ImageRenderer;
    class SoundPlayer;
    struct SoundPlayer::SoundInfo;
}


namespace EpicTool
{
    class DebugWindow;
    class GraphicWindow;
    enum DebugType;
 

    class InspectorWindow : public IWindow
    {
    public:
        InspectorWindow();
        ~InspectorWindow();

    public:
        void Initialize();

        virtual void ShowWindow() override;
        virtual void ShowWindow(bool* _open, std::vector<Object>& object) override;

        void ShowWindow(int& selectedObjectIndex);

        template<typename PieceType> // 특수화를 통해 컴포넌트의 조각을 만듬
        void DrawComponentPiece(PieceType& data, const std::string name); // 조각을 만들어 두고 갖다 쓰자

        // 종화형이 준 팁으로는 일단 템플릿을 통해 기본 뼈대를 모두 정의해두고 그것을 가져다 만드는 것이 좋다고 했다
        template<typename ComponentType>
        void DrawComponentInfo(ComponentType* instance);

        void DeleteComponent(KunrealEngine::Component* instance);

        void SetSelectObject(); // 선택된 오브젝트를 조작할 때 사용

        void IsCheckItem(bool& Item);

        void SetMeshObjectEditor(std::vector<std::string> & meshList, int selectedItem);
      
        template<typename ComponentType>
        void ComboControl(std::vector<std::string> list, std::vector<std::string> listEditor, int& selected, const char* name, ComponentType* instance);

        void ComboControlSound(std::vector<KunrealEngine::SoundPlayer::SoundInfo> list, std::vector<KunrealEngine::SoundPlayer::SoundInfo> listEditor, int& selected, const char* name);

        // 사운드 임포를 조절하는 함수

        void SetSoundInfo(KunrealEngine::SoundPlayer::SoundInfo& instance);

        void SoundCreate();

    public:
        void GetDebugType(DebugType& instance);

        void GetDeleteComponentName(std::string& componentName);

        // 하이어라키의 리스트와 연동
    public:
        void SetGameObjectList(std::vector<KunrealEngine::GameObject*>& intance);

    private:
        void ListToRemove(std::vector<std::string>& list, std::vector<std::string>& listEditor, const std::string stringToRemove);

        void StringRemove(std::string& listStr , const std::string removeString);
   
    private: 
        void RenderListBoxVector(const char* label, int* current_item, std::vector<std::string>& items);

    private:
        std::vector<KunrealEngine::GameObject*> _gameObjectlist;

        std::vector<KunrealEngine::Component*> _compoenetList;

        std::vector<std::string> _lightList;

        int _selectedObjectIndex;
        int _selectedObjectIndex2;

    private:
        // 컴포넌트를 쓰기 위한 임의 변수
        // 해당 변수를 통해 컴포넌트를 부른다
        KunrealEngine::Transform* _tranform;

        KunrealEngine::MeshRenderer* _meshRenderer;

        KunrealEngine::Camera* _camera;

        KunrealEngine::Light* _light;

        KunrealEngine::ImageRenderer* _image;

        KunrealEngine::GraphicsSystem* _graphicsSystem;

        KunrealEngine::SoundPlayer* _soundPlayer;

       //std::vector<KunrealEngine::SoundPlayer*> _soundPlayerlist;

        bool _meshState;

        bool _cameraFix; // 카메라

        

        //int _selectedMesh = -1; // 메쉬 선택을 보여줄 변수

        //int _selectedNormal;


    private:
        // 콤보에서 선택한 부분을 판단하는 변수
        int _comboMeshSelect;

        int _comboNormalSelect; 

        int _comboDiffuseSelect;

        int _comboLightSelect;

        int _comboImageSelect;

        int _comboSoundSelect;

        int _comboNewSoundSelect;

       std::string _selectObjectName;
        int _selectObjectNumber;

        // 오브젝트의 활성화를 관리할 변수
        bool _isObjectActive;

       // bool _isMeshRenderActive;

        bool _isLightActive;

        bool _isCameraActive;

        bool _isImageActive;

        // 피킹을 판단할 변수
        bool _isPickedObject;

       // bool _IsSetPisckable;

        std::string _isPickedObjectName;

        // 메쉬에서 Diffuse와 Normal을 관리

        int _selectedDiffuse; // 언젠가 내가 이미 만들어뒀다 무엇인ㄴ


        int _selectedNormal;

        std::vector<std::string> _diffuseName;

        std::vector<std::string> _normalName;

        bool isDiffuseMax;

        bool isNormalMax;

        float _isInvisible;
    private:
        // 그래픽스에서 받아온 목록을 저장
        std::vector<std::string> _meshList;
          
        std::vector<std::string> _meshListEditor;

        std::vector<std::string> _textureList;

        std::vector<std::string> _TextureListEditor;

        const std::string _meshStringToRemove = "Resources/ASEFile/";

        const std::string _textureStringToRemove = "Resources/Textures/";
    
        const std::string _directionalLight = "DirectionalLight";

        const std::string _pointLight = "PointLight";

        const std::string _spotLight = "SpotLight";

    private:

       float _ambient[4];
       float _diffuse[4];
       float _specular[4];
       float _direction[3];

       float _pointAmbient[4];
       float _pointDiffuse[4];
       float _pointSpecular[4];
       float _pointRange;

       bool _lightGet;

	   float _lightOffset[3];

       DirectX::XMFLOAT4 _quaternion;

    private:
		DebugWindow* _debugWindow;  // 디버그 테스트용 객체
		DebugType _DebugType;

		std::string _deleteComponentName;

        // 애니메이터를 관리할 변수
    private:
        float _animationSpeed;
        float _animationProgress;

        // 콜라이더를 관리할 변수
    private:
        float _offset[3];
        float _boxSize[3];

     private:
		int _currentNormal = 0;
		int _currentDiffuse = 0;

     private:
        bool _isSound3DEditor;
        bool _isLoopSoundEditor;
        int _soundVolEditor;

        KunrealEngine::SoundPlayer::SoundInfo _controlSoundInfo;

        // 사운드 생성 할때 필요한 매개를 저장할 변수

        std::vector<std::string> _isNewSoundPathList;

        std::vector<std::string> _isNewSoundPathListEditor;

        std::string _newSoundName;

        bool _isNewSound3D;
        bool _isNewSoundLoop;
        int _isNewSoundVol;

        float _setTargetPosition[3];


     private:
		 float _velocityParticle;				// 방출 강도
		 bool _randomParticle;					// 랜덤으로 흩뿌리는지
		 float _fadeoutTimeParticle;			// 서서히 흐려지게하는 시간
		 float _lifeTimeParticle;				// 얼마 뒤에 사라지게 할건가
         float _colorParticle[3];		        // RGB		// 이 색을 추가하겠다
		 float _directionParticle[3];	        // 방출 방향
         float _sizeParticle[2];                // 사이즈
         float _rotationParticle[3];            // 로테이션
         float _AngleParticle[3];               // 앵글
         bool  _isParticleCamera;
         bool _isActiveParticle;
    };
}
