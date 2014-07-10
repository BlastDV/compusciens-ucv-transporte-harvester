#include "deviceconnector.h"
#include "ui_deviceconnector.h"

DeviceConnector::DeviceConnector(QWidget *parent): QWidget(parent), ui(new Ui::DeviceConnector)
{
    ui->setupUi(this);
    Reader= new Csp32Bridge(this);
}

DeviceConnector::~DeviceConnector()
{
    delete ui;
}

/* Esta clase actua como un inicializador para el objeto.
 * DEBE LLAMARSE CADA VEZ QUE SE DESEE HACER USO DE ELLA */
void DeviceConnector::InitObject()
{
    SelectedCOM=-1;

    CheckCOMPorts(); // Cuales puertos estan disponibles?
    BloquearComandos(); // Hasta que no haya un puerto seleccionado
}

void DeviceConnector::RestoreObject()
{
    // Tratemos de cerrar los puertos
    Reader->cspRestore();

    // Ahora dejemos el objeto lo mas limpio posible
    SelectedCOM=-1;
    ui->ResultsPrompt->clear();
    BloquearComandos();

    ui->COM1Button->setChecked(false);
    ui->COM2Button->setChecked(false);
    ui->COM3Button->setChecked(false);
    ui->COM4Button->setChecked(false);
}

/* Esta funcion nos permitira saber de antemano cuales puertos COM se encuentran activos,
 * de esta forma ofreceremos al usuario solo las opciones posibles */
void DeviceConnector::CheckCOMPorts()
{
    //Probemos el COM1
    if (!Reader->cspGetCommInfo(COM1) == SERIAL_RS232)
        ui->COM1Button->setEnabled(false);

    //Probemos el COM2
    if (!Reader->cspGetCommInfo(COM2) == SERIAL_RS232)
        ui->COM2Button->setEnabled(false);

    //Probemos el COM3
    if (!Reader->cspGetCommInfo(COM3) == SERIAL_RS232)
        ui->COM3Button->setEnabled(false);

    //Probemos el COM4
    if (!Reader->cspGetCommInfo(COM4) == SERIAL_RS232)
        ui->COM4Button->setEnabled(false);

}


//Esta funcion bloquea todos los botones de comandos si no se ha escogido un puerto COM
void DeviceConnector::BloquearComandos()
{
    ui->InitButton->setEnabled(false);
    ui->RestoreButton->setEnabled(false);
}

//Esta funcion desbloquea todos los botones de comandos si se ha escogido un puerto COM
void DeviceConnector::DesbloquearComandos()
{
    ui->InitButton->setEnabled(true);
    ui->RestoreButton->setEnabled(true);
}

/* Esta funcion habilitara/deshabilitara los botones de comandos dependiendo del escenario actual.
 * Esto permite guiar al usuario de forma mas directa a traves del proceso de configuracion del
 * dispositivo. */
void DeviceConnector::HabilitarComandos(QString scenario)
{
    if (scenario=="PUERTO_ESCOGIDO" || scenario=="PUERTO_LIBERADO")
    {
        ui->InitButton->setEnabled(true);
        ui->RestoreButton->setEnabled(false);
    }
    else if (scenario=="PUERTO_INICIADO")
    {
        ui->InitButton->setEnabled(false);
        ui->RestoreButton->setEnabled(true);
    }
}


/** Funciones de la interfaz **/
void DeviceConnector::on_COM1Button_clicked()
{
    if (ui->COM1Button->isChecked())
    {
        ui->COM2Button->setChecked(false);
        ui->COM3Button->setChecked(false);
        ui->COM4Button->setChecked(false);
        SelectedCOM= COM1;
        HabilitarComandos("PUERTO_ESCOGIDO");

        ui->ResultsPrompt->appendPlainText("Puerto COM 1 escogido.");
    }
    else
    {
        BloquearComandos();
        ui->ResultsPrompt->appendPlainText("Ningun puerto escogido.");
    }
}

void DeviceConnector::on_COM2Button_clicked()
{
    if (ui->COM2Button->isChecked())
    {
        ui->COM1Button->setChecked(false);
        ui->COM3Button->setChecked(false);
        ui->COM4Button->setChecked(false);
        SelectedCOM= COM2;
        HabilitarComandos("PUERTO_ESCOGIDO");

        ui->ResultsPrompt->appendPlainText("Puerto COM 2 escogido.");
    }
    else
    {
        BloquearComandos();
        ui->ResultsPrompt->appendPlainText("Ningun puerto escogido.");
    }
}

void DeviceConnector::on_COM3Button_clicked()
{
    if (ui->COM3Button->isChecked())
    {
        ui->COM1Button->setChecked(false);
        ui->COM2Button->setChecked(false);
        ui->COM4Button->setChecked(false);
        SelectedCOM= COM3;
        HabilitarComandos("PUERTO_ESCOGIDO");

        ui->ResultsPrompt->appendPlainText("Puerto COM 3 escogido.");
    }
    else
    {
        BloquearComandos();
        ui->ResultsPrompt->appendPlainText("Ningun puerto escogido.");
    }
}

void DeviceConnector::on_COM4Button_clicked()
{
    if (ui->COM4Button->isChecked())
    {
        ui->COM4Button->setChecked(true);
        ui->COM1Button->setChecked(false);
        ui->COM2Button->setChecked(false);
        ui->COM3Button->setChecked(false);
        SelectedCOM= COM4;
        HabilitarComandos("PUERTO_ESCOGIDO");

        ui->ResultsPrompt->appendPlainText("Puerto COM 4 escogido.");
    }
    else
    {
        BloquearComandos();
        ui->ResultsPrompt->appendPlainText("Ningun puerto escogido.");
    }
}

//Esto se ejecutara al presionar "Iniciar Puerto"
void DeviceConnector::on_InitButton_clicked()
{
    int Result= Reader->cspInit(SelectedCOM);

    if (Result!=STATUS_OK)
        ui->ResultsPrompt->appendPlainText("Error: no se ha podido iniciar el puerto COM.");
    else
    {
        HabilitarComandos("PUERTO_INICIADO");
        ui->ResultsPrompt->appendPlainText(QString("Puerto %1 iniciado correctamente.").arg(SelectedCOM + 1));

        // Y reportamos al log
        emit(RegistrarEvento(QString("ABRIO PUERTO %1").arg(SelectedCOM + 1)));
    }
}

//Esto se ejecutara al presionar "Restaurar Puerto", cerrando el puerto escogido
void DeviceConnector::on_RestoreButton_clicked()
{
    int Result= Reader->cspRestore();

    if (!Result==STATUS_OK)
        ui->ResultsPrompt->appendPlainText("Error: no se ha podido liberar el puerto COM escogido.");
    else
    {
        HabilitarComandos("PUERTO_LIBERADO");
        ui->ResultsPrompt->appendPlainText(QString("Puerto %1 liberado correctamente.").arg(SelectedCOM + 1));

        // Y reportamos al log
        emit(RegistrarEvento(QString("LIBERO PUERTO %1").arg(SelectedCOM + 1)));
    }
}

// Cuando el usuario decide cancelar, todo se restaura
void DeviceConnector::on_CancelButton_clicked()
{
    RestoreObject();

    emit CancelPressed();
}

// Cuando el usuario decide guardar, el proceso continua
void DeviceConnector::on_AcceptButton_clicked()
{
    emit AcceptPressed();
}
