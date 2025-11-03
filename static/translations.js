// AirLab - Translations
const translations = {
    en: {
        // Header
        title: "AirLab - Pneumatic Test Bench",
        debugMode: "Debug Mode - AirLab",
        
        // Buttons
        calibrate: "Calibrate",
        reset: "Reset",
        unit: "Unit",
        shutdown: "Shutdown Raspberry Pi",
        reboot: "Reboot Raspberry Pi",
        
        // Navigation
        backToMain: "Back to Main View",
        debugPage: "Debug Page",
        
        // Channel labels
        vacuum1: "Vacuum 1",
        vacuum2: "Vacuum 2",
        vacuum3: "Vacuum 3",
        vacuum4: "Vacuum 4",
        
        // Status
        statusOk: "OK",
        statusError: "ERROR",
        statusWarning: "WARNING",
        
        // Debug info
        rawValue: "Raw Value:",
        offset: "Offset:",
        instantPressure: "Instant Pressure:",
        samplesInAvg: "Samples in Avg:",
        
        // Info messages
        debugInfo: "Debug information updates at 2Hz",
        lastUpdate: "Last update:",
        
        // Calibration instructions
        calibrationTitle: "Calibration Instructions",
        calibrationText: "To calibrate: Expose all suction cups to atmospheric pressure (no vacuum), then click \"Calibrate All Channels\" on the main page to zero the readings.",
        movingAverage: "Moving average filter: 10 samples for stable readings",
        systemStatus: "System status: Make sure",
        isRunning: "is running!",
        
        // Confirmation dialogs
        shutdownConfirm: "Are you sure you want to shutdown the Raspberry Pi?\n\nThis will turn off the system completely.",
        rebootConfirm: "Are you sure you want to reboot the Raspberry Pi?\n\nThe system will restart and the connection will be temporarily lost.",
        resetConfirm: "Reset all calibration offsets to zero?",
        
        // Success/Error messages
        calibrationSuccess: "All channels calibrated to 0 mbar",
        calibrationFailed: "Calibration failed:",
        resetSuccess: "Calibration reset to factory defaults",
        resetFailed: "Reset failed:",
        shutdownSuccess: "Raspberry Pi is shutting down...",
        shutdownFailed: "Shutdown failed:",
        rebootSuccess: "Raspberry Pi is rebooting...",
        rebootFailed: "Reboot failed:",
        connectionError: "Connection error:",
        readError: "Failed to read data from EtherCAT master"
    },
    pt: {
        // Header
        title: "AirLab - Bancada de Testes Pneumáticos",
        debugMode: "Modo Debug - AirLab",
        
        // Buttons
        calibrate: "Calibrar",
        reset: "Resetar",
        unit: "Unidade",
        shutdown: "Desligar Raspberry Pi",
        reboot: "Reiniciar Raspberry Pi",
        
        // Navigation
        backToMain: "Voltar à Vista Principal",
        debugPage: "Página de Debug",
        
        // Channel labels
        vacuum1: "Vácuo 1",
        vacuum2: "Vácuo 2",
        vacuum3: "Vácuo 3",
        vacuum4: "Vácuo 4",
        
        // Status
        statusOk: "OK",
        statusError: "ERRO",
        statusWarning: "AVISO",
        
        // Debug info
        rawValue: "Valor Bruto:",
        offset: "Offset:",
        instantPressure: "Pressão Instantânea:",
        samplesInAvg: "Amostras na Média:",
        
        // Info messages
        debugInfo: "Informações de debug atualizadas a 2Hz",
        lastUpdate: "Última atualização:",
        
        // Calibration instructions
        calibrationTitle: "Instruções de Calibração",
        calibrationText: "Para calibrar: Exponha todas as ventosas à pressão atmosférica (sem vácuo), depois clique em \"Calibrar Todos os Canais\" na página principal para zerar as leituras.",
        movingAverage: "Filtro de média móvel: 10 amostras para leituras estáveis",
        systemStatus: "Estado do sistema: Certifique-se que",
        isRunning: "está em execução!",
        
        // Confirmation dialogs
        shutdownConfirm: "Tem certeza que deseja desligar o Raspberry Pi?\n\nIsto irá desligar o sistema completamente.",
        rebootConfirm: "Tem certeza que deseja reiniciar o Raspberry Pi?\n\nO sistema irá reiniciar e a conexão será temporariamente perdida.",
        resetConfirm: "Resetar todos os offsets de calibração para zero?",
        
        // Success/Error messages
        calibrationSuccess: "Todos os canais calibrados para 0 mbar",
        calibrationFailed: "Falha na calibração:",
        resetSuccess: "Calibração resetada para valores de fábrica",
        resetFailed: "Falha ao resetar:",
        shutdownSuccess: "Raspberry Pi está desligando...",
        shutdownFailed: "Falha ao desligar:",
        rebootSuccess: "Raspberry Pi está reiniciando...",
        rebootFailed: "Falha ao reiniciar:",
        connectionError: "Erro de conexão:",
        readError: "Falha ao ler dados do mestre EtherCAT"
    }
};

// Current language (default: English)
let currentLang = localStorage.getItem('airlab_language') || 'en';

// Get translation
function t(key) {
    return translations[currentLang][key] || key;
}

// Set language
function setLanguage(lang) {
    currentLang = lang;
    localStorage.setItem('airlab_language', lang);
    
    // Update active button
    document.querySelectorAll('.lang-btn').forEach(btn => {
        btn.classList.remove('active');
        if (btn.dataset.lang === lang) {
            btn.classList.add('active');
        }
    });
    
    // Trigger translation update
    updateTranslations();
}

// Update all text elements with translations
function updateTranslations() {
    // This function should be implemented in each HTML page
    // to update the specific elements
    if (typeof updatePageTranslations === 'function') {
        updatePageTranslations();
    }
}

// Initialize language on page load
document.addEventListener('DOMContentLoaded', function() {
    // Set active language button
    document.querySelectorAll('.lang-btn').forEach(btn => {
        if (btn.dataset.lang === currentLang) {
            btn.classList.add('active');
        }
    });
    
    // Update translations
    updateTranslations();
});
