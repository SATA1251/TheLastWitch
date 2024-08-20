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

        template<typename PieceType> // Ư��ȭ�� ���� ������Ʈ�� ������ ����
        void DrawComponentPiece(PieceType& data, const std::string name); // ������ ����� �ΰ� ���� ����

        // ��ȭ���� �� �����δ� �ϴ� ���ø��� ���� �⺻ ���븦 ��� �����صΰ� �װ��� ������ ����� ���� ���ٰ� �ߴ�
        template<typename ComponentType>
        void DrawComponentInfo(ComponentType* instance);

        void DeleteComponent(KunrealEngine::Component* instance);

        void SetSelectObject(); // ���õ� ������Ʈ�� ������ �� ���

        void IsCheckItem(bool& Item);

        void SetMeshObjectEditor(std::vector<std::string> & meshList, int selectedItem);
      
        template<typename ComponentType>
        void ComboControl(std::vector<std::string> list, std::vector<std::string> listEditor, int& selected, const char* name, ComponentType* instance);

        void ComboControlSound(std::vector<KunrealEngine::SoundPlayer::SoundInfo> list, std::vector<KunrealEngine::SoundPlayer::SoundInfo> listEditor, int& selected, const char* name);

        // ���� ������ �����ϴ� �Լ�

        void SetSoundInfo(KunrealEngine::SoundPlayer::SoundInfo& instance);

        void SoundCreate();

    public:
        void GetDebugType(DebugType& instance);

        void GetDeleteComponentName(std::string& componentName);

        // ���̾��Ű�� ����Ʈ�� ����
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
        // ������Ʈ�� ���� ���� ���� ����
        // �ش� ������ ���� ������Ʈ�� �θ���
        KunrealEngine::Transform* _tranform;

        KunrealEngine::MeshRenderer* _meshRenderer;

        KunrealEngine::Camera* _camera;

        KunrealEngine::Light* _light;

        KunrealEngine::ImageRenderer* _image;

        KunrealEngine::GraphicsSystem* _graphicsSystem;

        KunrealEngine::SoundPlayer* _soundPlayer;

       //std::vector<KunrealEngine::SoundPlayer*> _soundPlayerlist;

        bool _meshState;

        bool _cameraFix; // ī�޶�

        

        //int _selectedMesh = -1; // �޽� ������ ������ ����

        //int _selectedNormal;


    private:
        // �޺����� ������ �κ��� �Ǵ��ϴ� ����
        int _comboMeshSelect;

        int _comboNormalSelect; 

        int _comboDiffuseSelect;

        int _comboLightSelect;

        int _comboImageSelect;

        int _comboSoundSelect;

        int _comboNewSoundSelect;

       std::string _selectObjectName;
        int _selectObjectNumber;

        // ������Ʈ�� Ȱ��ȭ�� ������ ����
        bool _isObjectActive;

       // bool _isMeshRenderActive;

        bool _isLightActive;

        bool _isCameraActive;

        bool _isImageActive;

        // ��ŷ�� �Ǵ��� ����
        bool _isPickedObject;

       // bool _IsSetPisckable;

        std::string _isPickedObjectName;

        // �޽����� Diffuse�� Normal�� ����

        int _selectedDiffuse; // ������ ���� �̹� �����״� �����Τ�


        int _selectedNormal;

        std::vector<std::string> _diffuseName;

        std::vector<std::string> _normalName;

        bool isDiffuseMax;

        bool isNormalMax;

        float _isInvisible;
    private:
        // �׷��Ƚ����� �޾ƿ� ����� ����
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
		DebugWindow* _debugWindow;  // ����� �׽�Ʈ�� ��ü
		DebugType _DebugType;

		std::string _deleteComponentName;

        // �ִϸ����͸� ������ ����
    private:
        float _animationSpeed;
        float _animationProgress;

        // �ݶ��̴��� ������ ����
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

        // ���� ���� �Ҷ� �ʿ��� �Ű��� ������ ����

        std::vector<std::string> _isNewSoundPathList;

        std::vector<std::string> _isNewSoundPathListEditor;

        std::string _newSoundName;

        bool _isNewSound3D;
        bool _isNewSoundLoop;
        int _isNewSoundVol;

        float _setTargetPosition[3];


     private:
		 float _velocityParticle;				// ���� ����
		 bool _randomParticle;					// �������� ��Ѹ�����
		 float _fadeoutTimeParticle;			// ������ ��������ϴ� �ð�
		 float _lifeTimeParticle;				// �� �ڿ� ������� �Ұǰ�
         float _colorParticle[3];		        // RGB		// �� ���� �߰��ϰڴ�
		 float _directionParticle[3];	        // ���� ����
         float _sizeParticle[2];                // ������
         float _rotationParticle[3];            // �����̼�
         float _AngleParticle[3];               // �ޱ�
         bool  _isParticleCamera;
         bool _isActiveParticle;
    };
}
